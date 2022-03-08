# starts a simple HTTP-Server at (arg0: host) on (arg1: port) with (arg2: root) 

require 'webrick'
require 'shell'
require "shellwords" 

host = $*[0]
puts("host: "+host)
port = $*[1].to_i()
puts("port: "+$*[1])
root = $*[2] 
puts("root: "+root)

server = WEBrick::HTTPServer.new :Port => port, :DocumentRoot => root, :ServerName => host

trap 'INT' do server.shutdown end
server.start	
