require 'rspec-expectations'
require 'firewatir'
require 'cucumber/formatter/unicode'
require 'smartcard'
require 'win32api'

Before do
  # @browser ||= FireWatir::Firefox.new(:profile => "xpitest", :waitTime => 3)  
   @browser ||= FireWatir::Firefox.new(:waitTime => 3)  
end

After do
  @browser.close
  @browser = nil
end
