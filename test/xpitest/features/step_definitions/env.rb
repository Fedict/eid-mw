require 'spec'
require 'firewatir'
require 'cucumber/formatter/unicode'
require 'smartcard'
require 'win32api'

Before do
  include Spec::Matchers 
  
 # Watir::Browser.default = "firefox"
   @browser ||= FireWatir::Firefox.new(:profile => "xpitest", :waitTime => 3)  
end

After do
  @browser.close
  @browser = nil
end
