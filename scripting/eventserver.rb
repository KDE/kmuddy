class EventServer
   include KMuddy

   def initialize(port = 4567)
      begin
         @server = TCPServer.new('localhost', port.to_i)
      rescue StandardError => errormsg
         warn("EventServer.initialize: #{errormsg}")
      end
   end

   def accept
      return @server.accept
   end
end
