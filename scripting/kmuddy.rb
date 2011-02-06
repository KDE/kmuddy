#!/bin/env ruby

require 'socket'

module KMuddy
    ANSI = { "reset"     => "\e[0m",    "bold"       => "\e[1m",
                     "underline" => "\e[4m",    "blink"      => "\e[5m",
                     "reverse"   => "\e[7m",    "invisible"  => "\e[8m",
                     "black"     => "\e[0;30m", "darkgrey"   => "\e[1;30m",
                     "red"       => "\e[0;31m", "lightred"   => "\e[1;31m",
                     "green"     => "\e[0;32m", "lightgreen" => "\e[1;32m",
                     "brown"     => "\e[0;33m", "yellow"     => "\e[1;33m",
                     "blue"      => "\e[0;34m", "lightblue"  => "\e[1;34m",
                     "purple"    => "\e[0;35m", "magenta"    => "\e[1;35m",
                     "cyan"      => "\e[1;36m", "lightcyan"  => "\e[1;36m",
                     "grey"      => "\e[0;37m", "white"      => "\e[1;37m",
                     "bgblack"   => "\e[40m",   "bgred"      => "\e[41m",
                     "bggreen"   => "\e[42m",   "bgyellow"   => "\e[43m",
                     "bgblue"    => "\e[44m",   "bgmagenta"  => "\e[45m",
                     "bgcyan"    => "\e[46m",   "bgwhite"    => "\e[47m"
                   }

   def ansi(text)
      ANSI[text]
   end

   def ansi_strip(line)
      return line.gsub!(/\e\[[0-9;]+m/, "")
   end

   def debug(text)
      #$stderr.puts("#{ansi("red")}--> #{text}#{ansi("reset")}")
      $stderr.puts("--> #{text}")
   end

   def warn(text)
      $stderr.puts("-! #{text}")
   end

   def output(text)
      $stderr.puts(text)
   end
end
