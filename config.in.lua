BUILD = path.isfile('Makefile') or path.isfile('CMakeLists.txt')
LOGDIR = path.current()
if os.linux or os.bsd then
	local logdir = '/var/log'
	if not BUILD and path.isdir(logdir) then
		LOGDIR = logdir
	end
end

server = {
--	The "listen" directive specifies address and port to listen on.
--	IPv6 addresses must be enclosed in square brackets.
--	
--	Examples: "127.0.0.1:80", "[::1]:80", "[1d7e:03ab:9b8c:cc5e:609b:d7fa:f38d:ea42]:80"
	['listen'] = '127.0.0.1:8080',
	
--	Worker process will be run with permissions of this user.
--	On Unix systems running process with permissions of another user does not require a password, but require root.
--	On Windows this directive has no effect, except for additional runtime checks. On Windows, you should start the process as this user manually, otherwise you will get an error (not critical, so that the process will continue to work).
--	
--	Examples: "www-data"
	['user'] = 'nobody',
	
--	Worker process will be run with permissions of this group.
--	On Windows this directive has no effect.
--	
--	Examples: "www-data"
	['group'] = 'nobody',
	
--	Log file.
	['log-file'] = path.join(LOGDIR, '@UNIX_PROG_NAME@.log')
}