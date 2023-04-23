#include <memory>
#include <cmath>
#include <complex>
#include <vector>
#include <cassert>
#include <climits>
#include <algorithm>
#include <stdio.h>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include "QuantumComputation.hpp"
#include "CircuitPartitioner.hpp"


namespace graph
{

using namespace qc;

using QubitCount = std::size_t;

typedef Eigen::MatrixXd Matrix;

bool cmp(Vertex *v1, Vertex *v2)
{
	return v1->eVec() < v2->eVec();
}

Vertex::Vertex(Qubit qubit)
{
	id_       = qubit;
	fs_      = 0;
	eVec_ = 0;
	weight_   = 0;
	isFixed_ = false;
}

Edge::Edge(Vertex* control, Vertex* target, int weight)
{
	control_ = control;
	target_  = target;
	weight_  = weight;
}

Graph::Graph(QuantumComputation& qc)
{
	QubitCount nQubit = qc.getNqubits();

	int** edge_table = new int*[nQubit];
	for(Qubit i = 0; i < nQubit; i++)
		edge_table[i] = new int[nQubit];

	for(Qubit i = 0; i < nQubit; i++)
	{
		Vertex v(i);
		vertices_.push_back(v);
		for(Qubit j = 0; j < nQubit; j++) 
			edge_table[i][j] = 0; // initialize edge_table
	}

	for(Qubit i = 0; i < nQubit; i++)
	{
		vertexMap_[i] = &(vertices_[i]);
		vertexPtrs_.push_back( &(vertices_[i]) );
	}

	auto it = qc.begin();
	while( it != qc.end() )
	{
		if((*it)->getNtargets() == 1 && (*it)->getNcontrols() == 1) 
		{
			int target  = static_cast<int>((*it)->getTargets()[0]);
			int control = static_cast<int>((*it)->getControls().begin()->qubit);

			assert(control != target);

			edge_table[control][target] += 1;
			edge_table[target][control] += 1;
		}
		++it;
	}

	for(Qubit i = 0; i < nQubit; i++)
	{
		for(Qubit j = i; j < nQubit; j++)
		{
			if(edge_table[i][j] != 0)
			{
				Vertex* c = vertexMap_[i];
				Vertex* t = vertexMap_[j];
				Edge e(c, t, edge_table[i][j]);
				edges_.push_back(e);
			}
		}
	}

	for(auto& e : edges_)
	{
		edgePtrs_.push_back(&e);
		e.control()->addEdge(&e);
		e.target()->addEdge(&e);
	}

	for(auto & v : vertices_)
		v.updateWeight();

	for(Qubit i = 0; i < nQubit; i++)
		delete [] edge_table[i];

	delete [] edge_table;
}

} // namespace graph

namespace qc
{

using namespace graph;

CircuitPartitioner::CircuitPartitioner(QuantumComputation& qc, double balance)
{
	graph_ = std::make_unique<Graph>(qc);

	lb_ = std::min(balance, 1 - balance);
	ub_ = std::max(balance, 1 - balance);

	//std::cout << "[Partitioner] Number of Edges: " << graph_->edges().size() << std::endl;
	//std::cout << "[Partitioner] Number of Vertices: " << graph_->vertices().size() << std::endl;

	for(auto& v : part1_)
		partL_.insert(v->id());
	for(auto& v : part2_)
		partU_.insert(v->id());

	if(*(partL_.begin()) != 0)
	{
		std::set<Qubit> temp;
		temp   = partL_;
		partL_ = partU_;
		partU_ = temp;
	}
}

bool
CircuitPartitioner::graphPartitioning(QuantumComputation& qc)
{
	int halfCut = halfSlicing();
	//std::cout << "[Partitioner] HalfCut: " << halfCut << std::endl;

	int initialCut = initialPartitioning();
	//std::cout << "[Partitioner] InitialCut: " << initialCut << std::endl;

	spectralPartitioning();
	applyFM(); // refining

	int finalCut = getTotalCut();
	int graphPartCut = finalCut;
	//std::cout << "[Partitioner] GraphPartCut: " << graphPartCut << std::endl;

	// if partitioning does not do better than
	// simple half-slicing,
	// we don't have to do partitioning
	bool partitionSuccess = false;
	if(initialCut <= graphPartCut && initialCut < halfCut)
	{
		partitionSuccess = true;
		finalCut = initialPartitioning();
	}
	else if(graphPartCut < initialCut && graphPartCut < halfCut)
	{
		partitionSuccess = true;
	}

	if(partitionSuccess)
	{
		for(auto& v : part1_) 
			partL_.insert(v->id());
		for(auto& v : part2_)
			partU_.insert(v->id());

		// HSFSimulator.cpp uses size of partL as a index for 
		// circuit slicing (sliceQubit)
		// Therefore, we have to make partL have q0 
		if(*(partL_.begin()) != 0)
		{
			std::set<Qubit> temp;
			temp   = partL_;
			partL_ = partU_;
			partU_ = temp;
		}

		Qubit idx1 = 0;
		for(auto v : partL_)
		{
			if(v != idx1)
			{
				initToPart_[v] = idx1;
				partToInit_[idx1] = v;
			}
			idx1++;
		}

		Qubit idx2 = static_cast<Qubit>(partL_.size());
		for(auto v : partU_)
		{
			if(v != idx2)
			{
				initToPart_[v] = idx2;
				partToInit_[idx2] = v;
			}
			idx2++;
		}

		if(initToPart_.size() != 0)
    {
			auto it = qc.begin();
			while( it != qc.end() )
			{
				if((*it)->getNcontrols() > 0) 
				{
					Qubit control = (*it)->getControls().begin()->qubit;
					if(initToPart_.count(control))
						(*it)->setControls({Control{initToPart_[control]}});
				}
	
				auto numTarget = (*it)->getNtargets();
				Targets targets;
				for(auto i = 0; i < numTarget; i++)
				{
					Qubit target  = (*it)->getTargets()[i];
					if(initToPart_.count(target))
						targets.push_back(initToPart_[target]);
				}
	
				if(!targets.empty())
					(*it)->setTargets(targets);
	
				it++;
			}
		}

		// partToInit is different with swapList
		std::vector<Qubit> qubitListAfterPartition(0);
		for(auto q : partL_)
			qubitListAfterPartition.push_back(q);
		for(auto q : partU_)
			qubitListAfterPartition.push_back(q);

		Qubit ptr1 = 0, ptr2 = 0;
		int nQubit = partL_.size() + partU_.size();
		while(ptr1 < nQubit)
		{
			if(qubitListAfterPartition[ptr1] != ptr1)
			{
				ptr2 = qubitListAfterPartition[ptr1];
				Qubit temp = qubitListAfterPartition[ptr2];
				swapList_.push_back(std::make_pair(ptr1, ptr2));
				qubitListAfterPartition[ptr1] = temp;
				qubitListAfterPartition[ptr2] = ptr2;
				ptr1 = 0;
			}
			else
				ptr1++;
		}

//		std::cout << std::endl;
//
//		std::cout << "InitToPart" << std::endl;
//		for(auto kv : initToPart_)
//			std::cout << kv.first << " <-> " << kv.second << std::endl;
//
//		std::cout << "PartToInit" << std::endl;
//		for(auto kv : partToInit_)
//			std::cout << kv.first << " <-> " << kv.second << std::endl;
//
//		std::cout << "Swap List " << swapList_.size() << std::endl;
//		for(auto kv : swapList_)
//			std::cout << kv.first << " <-> " << kv.second << std::endl;
//
//		std::cout << std::endl;
//
//		std::cout << "Partition L Size: " << partL_.size() << " |  ";
//		for(Qubit q : partL_)
//			std::cout << q << " ";
//		std::cout << std::endl;
//
//		std::cout << "Partition U Size: " << partU_.size() << " |  ";
//		for(Qubit q : partU_)
//			std::cout << q << " ";
//		std::cout << std::endl;
	}

	//std::cout << "[Partitioner] FinalCut: " << finalCut << std::endl;

	return partitionSuccess;
}

void
CircuitPartitioner::spectralPartitioning()
{
	QubitCount nQubit = graph_->vertices().size();

	// L := Laplacian of G == D_G - A_G
	Matrix Laplacian = Eigen::MatrixXd::Constant(nQubit, nQubit, 0.0);

	// Compute D_G := Degree Matrix
	for(auto v : graph_->vertices())
	{
		int qubit     = static_cast<int>(v->id());
		double weight = static_cast<double>(v->weight());
		Laplacian(qubit, qubit) = weight;
	}

	// Compute A_G := Adjacency Matrix
	for(auto e : graph_->edges())
	{
		int control   = static_cast<int>(e->control()->id());
		int target    = static_cast<int>(e->target()->id());
		double weight = static_cast<double>(-1 * e->weight());
		Laplacian(control, target) = weight;
		Laplacian(target, control) = weight;
	}

	// Compute Eigenvalues of the Laplacian
	// Eigen::EigenSolver<Matrix> solver(K.inverse() * Laplacian);
	Eigen::EigenSolver<Matrix> solver(Laplacian);

	auto eVal  = solver.eigenvalues();
	auto eVec  = solver.eigenvectors();

	// Find the 2nd smallest eigen value
	double min1 = std::numeric_limits<double>::max(); // For min
	double min2 = std::numeric_limits<double>::max(); // For 2nd min

	int min1_idx = 0;
	int min2_idx = 0;

	for(size_t i = 0; i < eVal.size(); i++)
	{
		double val = real(eVal[i]);
		if(min2 > val && val > min1)
		{
			min2     = val;
			min2_idx = i;
		}
		else if(min2 > val && min1 > val)
		{
			min2     = min1;
			min2_idx = min1_idx;
			min1     = val;
			min1_idx = i;
		}
	}

	// Try the partition based on the sorted eigenvector
	std::vector<Vertex*> sortedQubit; 
	for(size_t i = 0; i < eVec.cols(); i++)
	{
		Vertex* v = graph_->getVertex(static_cast<Qubit>(i));
		v->setEigenVec(real(eVec(i, min2_idx)));
		sortedQubit.push_back(v);
	}

	std::sort(sortedQubit.begin(), sortedQubit.end(), cmp);

	QubitCount nQubitLB 
		= static_cast<QubitCount>(std::ceil(lb_ * static_cast<double>(nQubit)));
	QubitCount nQubitUB
		= static_cast<QubitCount>(std::floor(ub_ * static_cast<double>(nQubit)));

	part1_.clear();
	part2_.clear();

	QubitCount bestPartition;
	int minCut = INT_MAX;

	for(auto i = nQubitLB; i < nQubitUB + 1; i++)
	{
		for(auto j = 0; j < i; j++)
			part1_.insert(sortedQubit[j]);
		for(auto j = i; j < nQubit; j++)
			part2_.insert(sortedQubit[j]);

		int numCut = getTotalCut();

		if(numCut < minCut) 
		{
			minCut = numCut;
			bestPartition = i;
		}

		part1_.clear();
		part2_.clear();
	}

	for(QubitCount i = 0; i < bestPartition; i++)
		part1_.insert(sortedQubit[i]);

	for(QubitCount i = bestPartition; i < nQubit; i++)
		part2_.insert(sortedQubit[i]);
}

void
CircuitPartitioner::applyFM()
{
	// Step1: Initial Partitioning -> from Spectral Partitioning
	for (auto& v : graph_->vertices())
	{
		freeVertices_.insert(v);
	}

	bestPartL_ = part1_;
	bestPartU_ = part2_;

	// Step2: Initialization
	for(auto& v : graph_->vertices())
	{
		updateFS(v);
		updateWeight(v);
	}

	int gainSum = 0;

	// Step3: Main Loop
	while(!freeVertices_.empty())
	{
		Vertex* maxVertex = nullptr;
		maxVertex = findMaxGain();

		if(maxVertex) tryMove(maxVertex, gainSum);
		else          break;

		orderList_.push_back(std::make_pair(gainSum, maxVertex));

		for(auto& v : criticalVertices_)
		{
			updateFS(v);
			updateWeight(v);
		}
	}

	// Step4: Find the best move in the OrderList
	int bestGainSum = 0;
	int bestGainIdx = 0;
	for(size_t i = 0; i < orderList_.size(); i++)
	{
		if(orderList_[i].first > bestGainSum)
		{
			bestGainIdx = i+1;
			bestGainSum = orderList_[i].first;
		}
	}

	// Step5: Execute the best move
	for(size_t i = 0; i < bestGainIdx; i++)
	{
		Vertex* v = orderList_[i].second;	
		realMove(v);
	}

	// Step6: Make the Final partitions
	// These classes will be used 
	// to synchronize with QuantumComputation Class
	part1_ = bestPartL_;
	part2_ = bestPartU_;
}

Vertex*
CircuitPartitioner::findMaxGain()
{
	int MAX_GAIN = INT_MIN;
	Vertex* maxVertex = nullptr;

	for(auto& v : graph_->vertices())
	{
		bool breakTie = false;

		if(checkBalance(v) && !v->isFixed())
		{
			if(v->gain() >= MAX_GAIN) breakTie = true;
			else if(v->gain() == MAX_GAIN)
			{
				// Tie-Breaking Policy
				int sizeL = static_cast<int>(part1_.size());
				int sizeU = static_cast<int>(part2_.size());

				if(part1_.count(v))
				{
					if(sizeL > sizeU) breakTie = true;
				}
				else if(part2_.count(v))
				{
					if(sizeU > sizeL) breakTie = true;
				}
			}

			if(breakTie)
			{
				MAX_GAIN = v->gain();
				maxVertex = v;	
			}
		}
	}

	return maxVertex;
}

int
CircuitPartitioner::halfSlicing()
{
	QubitCount nQubit = graph_->vertices().size();

	for (auto& v : graph_->vertices())
	{
		if(v->id() < static_cast<Qubit>(nQubit/2)) 
			part1_.insert(v);
		else                                       
			part2_.insert(v);
	}

	int halfCut = getTotalCut();

	return halfCut;
}

int
CircuitPartitioner::initialPartitioning()
{
	QubitCount bestPartition;
	int minCut = INT_MAX;

	QubitCount nQubit = graph_->vertices().size();
	QubitCount nQubitLB 
		= static_cast<QubitCount>(std::ceil(lb_ * static_cast<double>(nQubit)));
	QubitCount nQubitUB
		= static_cast<QubitCount>(std::floor(ub_ * static_cast<double>(nQubit)));

	for(auto i = nQubitLB; i < nQubitUB + 1; i++)
	{
		part1_.clear();
		part2_.clear();

		for(auto j = 0; j < i; j++)
			part1_.insert(graph_->getVertex(static_cast<Qubit>(j)));

		for(auto j = i; j < nQubit; j++)
			part2_.insert(graph_->getVertex(static_cast<Qubit>(j)));

		int numCut = getTotalCut();

		if(numCut < minCut) 
		{
			minCut = numCut;
			bestPartition = i;
		}
	}

	part1_.clear();
	part2_.clear();

	for(auto i = 0; i < bestPartition; i++)
		part1_.insert(graph_->getVertex(static_cast<Qubit>(i)));

	for(auto i = bestPartition; i < nQubit; i++)
		part2_.insert(graph_->getVertex(static_cast<Qubit>(i)));

	int initialCut = getTotalCut();

	return initialCut;
}

bool
CircuitPartitioner::checkBalance(Vertex* v)
{
	int sizeL = static_cast<int>(part1_.size());
	int sizeU = static_cast<int>(part2_.size());

	int total = sizeL + sizeU;

	if(part1_.count(v))
	{
		double lb = static_cast<double>(std::min(sizeL - 1, sizeU + 1)) 
			        / static_cast<double>(total);   

		double ub = static_cast<double>(std::max(sizeL - 1, sizeU + 1)) 
			        / static_cast<double>(total);   

		if(lb < lb_ || ub > ub_) return false;
	}
	else if(part2_.count(v))
	{
		double lb = static_cast<double>(std::min(sizeL + 1, sizeU - 1)) 
			        / static_cast<double>(total);   

		double ub = static_cast<double>(std::max(sizeL + 1, sizeU - 1)) 
			        / static_cast<double>(total);   

		if(lb < lb_ || ub > ub_) return false;
	}

	return true;
}

void
CircuitPartitioner::tryMove(Vertex* v, int& gainSum)
{
	criticalVertices_.clear();

	if(part1_.count(v))
	{
		part1_.erase(v);
		part2_.insert(v);
	}
	if(part2_.count(v))
	{
		part2_.erase(v);
		part1_.insert(v);
	}

	for(auto& e : v->edges())
	{
		criticalVertices_.insert(e->control());
		criticalVertices_.insert(e->target());
	}

	v->setFixed();
	freeVertices_.erase(v);
	gainSum += v->gain();
}

void
CircuitPartitioner::realMove(Vertex* v)
{
	criticalVertices_.clear();

	if(bestPartL_.count(v))
	{
		bestPartL_.erase(v);
		bestPartU_.insert(v);
	}
	if(bestPartU_.count(v))
	{
		bestPartU_.erase(v);
		bestPartL_.insert(v);
	}
}

bool
CircuitPartitioner::checkCut(Edge* e)
{
	bool isCut = false;

	if(part1_.count( e->control() ) )
	{
		if(part1_.count( e->target() ) ) isCut = false;
		else                             isCut = true;
	}
	else if(part2_.count( e->control() ) )
	{
		if(part2_.count( e->target() ) ) isCut = false;
		else                             isCut = true;
	}

	return isCut;
}

// FS = Total weights of connected-cut-edges
void 
CircuitPartitioner::updateFS(Vertex* v)
{
	int fs = 0;
	for(auto& e : v->edges())
		if(checkCut(e)) fs += e->weight();

	v->setFS(fs);
}

// Weight = Total weights of connected-edges
void 
CircuitPartitioner::updateWeight(Vertex* v)
{
	int w = 0;
	for(auto& e : v->edges())
		w += e->weight();

	v->setWeight(w);
}

int
CircuitPartitioner::getTotalCut()
{
	int cut = 0;
	for(auto e : graph_->edges())
		if(checkCut(e)) cut += e->weight();

	return cut;
}

void
CircuitPartitioner::restoreQuantumComputation(QuantumComputation& qc)
{
	if(partToInit_.size() == 0) 
		return;

	auto it = qc.begin();
	while( it != qc.end() )
	{
		if((*it)->getNcontrols() > 0) 
		{
			Qubit control = (*it)->getControls().begin()->qubit;
			if(partToInit_.count(control))
				(*it)->setControls({Control{partToInit_[control]}});
		}

		auto numTarget = (*it)->getNtargets();
		Targets targets;
		for(auto i = 0; i < numTarget; i++)
		{
			Qubit target  = (*it)->getTargets()[i];
			if(partToInit_.count(target))
				targets.push_back(partToInit_[target]);
		}

		if(!targets.empty())
			(*it)->setTargets(targets);

		it++;
	}
}

} // namespace qc
