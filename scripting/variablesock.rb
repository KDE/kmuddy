class VariableSock
   include KMuddy

   def initialize
      if ENV.has_key?('KMUDDY_SOCKET')
         @socket_name = ENV['KMUDDY_SOCKET']
      else
         warn("Variable KMUDDY_SOCKET is not set - variable support won't work!")
      end

      begin
         @socketfile = UNIXSocket.new(@socket_name)
      rescue StandardError => errormsg
         warn("VariableSock.initialize: #{errormsg}")
      end
   end

   def close
      unless(@socketfile.closed?)
         @socketfile.close
      else
         warn("VariableSock.close: The pipe is not open.")
      end
   end

   def get(name)
      unless(@socket_name.nil? || @socket_name.empty?)
         begin
            @socketfile.puts("get #{name}")
            return @socketfile.gets.chomp
         rescue StandardError => errormsg
            warn("VariableSock.get: #{errormsg}")
         end
      else
         warn("VariableSock.get: The pipe is not open.")
      end
   end

   def set(name,value)
      unless(@socket_name.nil? || @socket_name.empty?)
         begin
            @socketfile.puts("set #{name} #{value}")
            return @socketfile.gets.chomp
         rescue StandardError => errormsg
            warn("VariableSock.set: #{errormsg}")
         end
      else
         warn("VariableSock.set: The pipe is not open.")
      end
   end

   def unset(name)
      unless(@socket_name.nil? || @socket_name.empty?)
         begin
            @socketfile.puts("unset #{name}")
            return @socketfile.gets.chomp
         rescue StandardError => errormsg
            warn("VariableSock.unset: #{errormsg}")
         end
      else
         warn("VariableSock.unset: The pipe is not open.")
      end
   end

   def inc(name)
      unless(@socket_name.nil? || @socket_name.empty?)
         begin
            @socketfile.puts("inc #{name}")
            return @socketfile.gets.chomp
         rescue StandardError => errormsg
            warn("VariableSock.inc: #{errormsg}")
         end
      else
         warn("VariableSock.inc: The pipe is not open.")
      end
   end

   def dec(name)
      unless(@socket_name.nil? || @socket_name.empty?)
         begin
            @socketfile.puts("dec #{name}")
            return @socketfile.gets.chomp
         rescue StandardError => errormsg
            warn("VariableSock.dec: #{errormsg}")
         end
      else
         warn("VariableSock.dec: The pipe is not open.")
      end
   end

   def request(name)
      unless(@socket_name.nil? || @socket_name.empty?)
         begin
            @socketfile.puts("request #{name}")
            return @socketfile.gets.chomp
         rescue StandardError => errormsg
            warn("VariableSock.request: #{errormsg}")
         end
      else
         warn("VariableSock.request: The pipe is not open.")
      end
   end

   def provide(name)
      unless(@socket_name.nil? || @socket_name.empty?)
         begin
            @socketfile.puts("provide #{name}")
            return @socketfile.gets.chomp
         rescue StandardError => errormsg
            warn("VariableSock.provide: #{errormsg}")
         end
      else
         warn("VariableSock.provide: The pipe is not open.")
      end
   end

   def lock(name)
      unless(@socket_name.nil? || @socket_name.empty?)
         begin
            @socketfile.puts("lock #{name}")
            return @socketfile.gets.chomp
         rescue StandardError => errormsg
            warn("VariableSock.lock: #{errormsg}")
         end
      else
         warn("VariableSock.lock: The pipe is not open.")
      end
   end

   def unlock(name)
      unless(@socket_name.nil? || @socket_name.empty?)
         begin
            @socketfile.puts("unlock #{name}")
            return @socketfile.gets.chomp
         rescue StandardError => errormsg
            warn("VariableSock.unlock: #{errormsg}")
         end
      else
         warn("VariableSock.unlock: The pipe is not open.")
      end
   end

   def command(command)
      unless(@socket_name.nil? || @socket_name.empty?)
         begin
            @socketfile.puts("send #{command}")
            @socketfile.gets.chomp
            @socketfile.puts("set lastcommand #{command}")
            return @socketfile.gets.chomp
         rescue StandardError => errormsg
            warn("VariableSock.command: #{errormsg}")
         end
      else
         warn("VariableSock.command: The pipe is not open.")
      end
   end
end
