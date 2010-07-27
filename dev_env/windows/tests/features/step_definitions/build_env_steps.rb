begin require 'rspec/expectations'; rescue LoadError; require 'spec/expectations'; end
require 'cucumber/formatter/unicode'
require 'tmpdir'

Before do
end

After do
end

Given /I am on a clean Windows system/ do 
#	RUBY_PLATFORM =~ /mswin32/
# do something like 
# * removing files
# * ask to the host to restore this guest VM in a "clean" state
end

When /I run the build_env installer script (.+)/ do |url|
	result = system "powershell " +
						"Set-ExecutionPolicy Unrestricted; " + 
	                    "Import-Module BitsTransfer; " +
	                    "New-Item  c:\\tmp\ -ItemType Directory -ErrorAction SilentlyContinue; " +
	                    "Start-BitsTransfer -Source " + url + " -Destination c:\\tmp\\build_env.ps1; " +
	                    "c:\\tmp\\build_env.ps1" 
end

Then /I should be able to check out from trunk/ do
	@checkoutdir = Dir.tmpdir + "/eid-mw" + rand(10000).to_s
	puts "checkoutdir: " + @checkoutdir
	Dir.mkdir @checkoutdir
	result = system "svn checkout http://eid-mw.googlecode.com/svn/trunk/ " + @checkoutdir 
	result.should == true
	
end

Then /I should be to build the trunk/ do
	puts "sh -c \"cd \\\"" + @checkoutdir + "\\\";" +
							"autoreconf -i -f;\""
	result = system "sh -c \"cd \\\"" + @checkoutdir + "\\\";" +
							"autoreconf -i -f;\"" 
	result.should == true
end


Then /the result should be (.*) on the screen/ do |result|

end

