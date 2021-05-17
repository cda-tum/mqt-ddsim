from concurrent import futures
import logging
import functools

from qiskit.providers import JobV1 as Job
from qiskit.providers import JobStatus, JobError

logger = logging.getLogger(__name__)


def requires_submit(func):
    """
    Decorator to ensure that a submit has been performed before
    calling the method.
    Args:
        func (callable): test function to be decorated.
    Returns:
        callable: the decorated function.
    """
    @functools.wraps(func)
    def _wrapper(self, *args, **kwargs):
        if self._future is None:
            raise JobError("Job not submitted yet!. You have to .submit() first!")
        return func(self, *args, **kwargs)
    return _wrapper


class JKQJob(Job):
    """AerJob class.
    Attributes:
        _executor (futures.Executor): executor to handle asynchronous jobs
    """

    _executor = futures.ThreadPoolExecutor(max_workers=1)

    def __init__(self, backend, job_id, fn, quantum_circuit, **args):
        super().__init__(backend, job_id)
        self._fn = fn
        self._quantum_circuit = quantum_circuit
        self._args = args
        self._future = None

    def submit(self):
        """Submit the job to the backend for execution.
        Raises:
            QobjValidationError: if the JSON serialization of the Qobj passed
            during construction does not validate against the Qobj schema.
            JobError: if trying to re-submit the job.
        """
        if self._future is not None:
            raise JobError("We have already submitted the job!")

        self._future = self._executor.submit(self._fn,
                                             self._job_id,
                                             self._quantum_circuit,
                                             **self._args)

    @requires_submit
    def result(self, timeout=None):
        # pylint: disable=arguments-differ
        """Get job result. The behavior is the same as the underlying
        concurrent Future objects,
        https://docs.python.org/3/library/concurrent.futures.html#future-objects
        Args:
            timeout (float): number of seconds to wait for results.
        Returns:
            qiskit.Result: Result object
        Raises:
            concurrent.futures.TimeoutError: if timeout occurred.
            concurrent.futures.CancelledError: if job cancelled before completed.
        """
        return self._future.result(timeout=timeout)

    @requires_submit
    def cancel(self):
        return self._future.cancel()

    @requires_submit
    def status(self) -> JobStatus:
        """Gets the status of the job by querying the Python's future
        Returns:
            JobStatus: The current JobStatus
        Raises:
            JobError: If the future is in unexpected state
            concurrent.futures.TimeoutError: if timeout occurred.
        """
        # The order is important here
        if self._future.running():
            _status = JobStatus.RUNNING
        elif self._future.cancelled():
            _status = JobStatus.CANCELLED
        elif self._future.done():
            _status = JobStatus.DONE if self._future.exception() is None else JobStatus.ERROR
        else:
            # Note: There is an undocumented Future state: PENDING, that seems to show up when
            # the job is enqueued, waiting for someone to pick it up. We need to deal with this
            # state but there's no public API for it, so we are assuming that if the job is not
            # in any of the previous states, is PENDING, ergo INITIALIZING for us.
            _status = JobStatus.INITIALIZING
        return _status

    def backend(self):
        """Return the instance of the backend used for this job."""
        return self._backend
