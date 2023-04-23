#pragma once

#include <Eigen/Core>
#include "Definitions.hpp"
#include "QuantumComputation.hpp"
#include "operations/Operation.hpp"

#include <set>
#include <memory>

namespace graph
{

using namespace qc;

using Qubit      = qc::Qubit;
using QubitCount = std::size_t;

class Vertex;

class Edge
{
	public:
		Edge(Vertex* control, Vertex* target, int weight);

		int weight() const { return weight_; }

		Vertex* control() const { return control_; }
		Vertex* target()  const { return target_;  }

	private:
		int weight_;

		Vertex* control_;
		Vertex* target_;

};

// FS = Total weights of connected-cut-edges
// Weight = Total weights of connected-edges
class Vertex
{
	public:
		Vertex(Qubit id); // id == qubit idx

		int fs()       const { return fs_;               }
		int weight()   const { return weight_;           }
		int gain()     const { return 2 * fs_ - weight_; }
		Qubit id()     const { return id_;               }
		bool isFixed() const { return isFixed_;          }
		int degree()   const { return static_cast<int>(edges_.size()); }

		void addEdge(Edge* edge) { edges_.insert(edge); }
		const std::set<Edge*>& edges() const { return edges_; }

		void updateWeight()
		{
			weight_ = 0;
			for(auto e : edges_)
				weight_ += e->weight();
		}

		double eVec() const { return eVec_; }
		void setEigenVec(double eVec) { eVec_ = eVec; }

		void setFixed()       { isFixed_ = true; }
		void setFS(int fs)    { fs_ = fs;        }
		void setWeight(int w) { weight_ = w;     }

	private:
		int fs_; // for FM algorithm
		int weight_;
		Qubit id_; // id == qubit idx
		double eVec_; // For Spectral Partitioning

		bool isFixed_;

		std::set<Edge*> edges_;
};


class Graph
{
	public:
		Graph(QuantumComputation& qc);

		const std::vector<Edge*>&   edges()    const { return edgePtrs_;    }
		const std::vector<Vertex*>& vertices() const { return vertexPtrs_; }

		Vertex* getVertex(Qubit q) { return vertexMap_[q]; }

	private:
		std::vector<Edge> edges_;
		std::vector<Vertex> vertices_;

		std::vector<Edge*> edgePtrs_;
		std::vector<Vertex*> vertexPtrs_;

		std::map<Qubit, Vertex*> vertexMap_;
};

} // namespace graph

namespace qc {

using namespace graph;

class CircuitPartitioner 
{
	public:
		CircuitPartitioner(QuantumComputation& qc, double balance);
		const std::map<Qubit, Qubit>& initToPart()   const { return initToPart_; }
		const std::map<Qubit, Qubit>& partToInit()   const { return partToInit_; }
		const std::vector<std::pair<Qubit, Qubit>>& swapList() const { return swapList_;   }

		const std::set<Qubit>& partL() const { return partL_; }
		const std::set<Qubit>& partU() const { return partU_; }

		Qubit getSplitQubit() const { return static_cast<Qubit>(partL_.size()); }

		bool graphPartitioning(QuantumComputation& qc);

		void restoreQuantumComputation(QuantumComputation& qc);

	private:

		int  halfSlicing();
		int  initialPartitioning();
		void spectralPartitioning();

		// Bi-Partition
		std::set<Vertex*> part1_;
		std::set<Vertex*> part2_; 

		std::set<Qubit> partL_;
		std::set<Qubit> partU_; 
		// there are used to update QuantumComputation

		// Sub-Procedures for FM
		void applyFM();
		Vertex* findMaxGain(); 
		bool checkBalance(Vertex* v);
		void tryMove(Vertex* v, int& gainSum);
		void realMove(Vertex* v);
		bool checkCut(Edge* e);
		void updateFS(Vertex* v);
		void updateWeight(Vertex* v);
		int  getTotalCut();
		std::vector<std::pair<int, Vertex*>> orderList_;

		std::set<Vertex*> bestPartL_;
		std::set<Vertex*> bestPartU_; 
		std::set<Vertex*> freeVertices_;
		std::set<Vertex*> criticalVertices_;

		// key: inital (== original) qubit
		// value: qubit after partition
		std::map<Qubit, Qubit> initToPart_;

		// key: qubit after partition
		// value: initial (== original) qubit
		std::map<Qubit, Qubit> partToInit_;

		std::vector<std::pair<Qubit, Qubit>> swapList_;

		double lb_;
		double ub_;
		std::unique_ptr<Graph> graph_;
};

} // namespace qc
