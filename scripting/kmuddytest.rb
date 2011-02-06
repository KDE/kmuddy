#!/usr/bin/env ruby
require 'kmuddy.rb'
require 'eventserver.rb'
require 'variablesock.rb'

include KMuddy     # not necessary in order to use KMuddy interface;
                     # I included this for easy access to the debug()
                     # function, which just writes to STDOUT.
$myport = 4567

evserver = EventServer.new($myport) # port can be anything.
varsock =  VariableSock.new()       # no special options required here.
threads = [ ]

# one thread for parsing STDIN, one for parsing "events".
threads << Thread.new {
   while line = STDIN.gets.chomp
      # Normally one would parse the line of text from the server here.
      # Instead, I demonstrate the 'set' method of the VariableSock.
      # Check your variables in KMuddy after you receive text from the
      # mud.
      varsock.set("lastline",line)
      debug(varsock.get("lastline"))
   end
}

threads << Thread.new {
   while (event = evserver.accept)
      line = event.gets.chomp
      debug(line) unless line.empty?
      exit(0) if line == "quit"
      varsock.command(line)
      event.close
   end
}

threads.each { |task| task.join }
