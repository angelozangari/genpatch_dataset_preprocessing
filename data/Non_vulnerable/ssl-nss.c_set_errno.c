static void
set_errno(int code)
{
	/* FIXME: this should handle more. */
	switch (code) {
	case PR_INVALID_ARGUMENT_ERROR:
		errno = EINVAL;
		break;
	case PR_PENDING_INTERRUPT_ERROR:
		errno = EINTR;
		break;
	case PR_IO_PENDING_ERROR:
		errno = EAGAIN;
		break;
	case PR_WOULD_BLOCK_ERROR:
		errno = EAGAIN;
		/*errno = EWOULDBLOCK; */
		break;
	case PR_IN_PROGRESS_ERROR:
		errno = EINPROGRESS;
		break;
	case PR_ALREADY_INITIATED_ERROR:
		errno = EALREADY;
		break;
	case PR_NETWORK_UNREACHABLE_ERROR:
		errno = EHOSTUNREACH;
		break;
	case PR_CONNECT_REFUSED_ERROR:
		errno = ECONNREFUSED;
		break;
	case PR_CONNECT_TIMEOUT_ERROR:
	case PR_IO_TIMEOUT_ERROR:
		errno = ETIMEDOUT;
		break;
	case PR_NOT_CONNECTED_ERROR:
		errno = ENOTCONN;
		break;
	case PR_CONNECT_RESET_ERROR:
		errno = ECONNRESET;
		break;
	case PR_IO_ERROR:
	default:
		errno = EIO;
		break;
	}
}
