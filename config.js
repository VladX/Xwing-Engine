var BUILD = path.isFile('Makefile') || path.isFile('CMakeLists.txt')
var LOGDIR = path.current()
if (os.linux || os.bsd) {
	var logdir = '/var/log'
	if (!BUILD && path.isDir(logdir)) {
		LOGDIR = logdir
	}
}

server = {
/*	The "listen" directive specifies address and port to listen on.
	IPv6 addresses must be enclosed in square brackets.
	For unix domain sockets specify 'file:' before actual file name.
	
	Examples: "127.0.0.1:80", "[::1]:80", "[1d7e:03ab:9b8c:cc5e:609b:d7fa:f38d:ea42]:80", "file:/tmp/xwing.sock"
*/
	'listen': '127.0.0.1:8080',
	
/*	Change permissions of a file assiciated with unix domain socket.
*/
	'socket-file-permissions': 00660,
	
/*	Worker process will be launched with permissions of this user.
	On Unix systems running processes with permissions of another user does not require a password, but require root.
	On Windows this directive has no effect, except for additional runtime checks. On Windows, you should start the process as this user manually, otherwise you will get an error (not critical, so that the process will continue to work).
	
	Examples: "www-data"
*/
	'user': 'nobody',
	
/*	Worker process will be launched with permissions of this group.
	On Windows this directive has no effect.
	
	Examples: "www-data"
*/
	'group': 'nobody',
	
/*	Log file.
*/
	'log-file': path.join(LOGDIR, 'xwing.log'),
	
/*	Run in daemon mode.
*/
	'detached': false
}
