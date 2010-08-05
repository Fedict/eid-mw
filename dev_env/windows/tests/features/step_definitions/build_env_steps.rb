begin require 'rspec/expectations'; rescue LoadError; require 'spec/expectations'; end
require 'cucumber/formatter/unicode'
require 'tmpdir'

Before do
	@checkoutdir = Dir.tmpdir + "/eid-mw" + rand(10000).to_s
	Dir.mkdir @checkoutdir
end

After do
	FileUtils.rm_r @checkoutdir	
end

Given /I am on a clean Windows system/ do 
#	RUBY_PLATFORM =~ /mswin32/
# do something like 
# * removing files 
# * ask to the host to restore this guest VM in a "clean" state
end

When /I run the build_env installer script (.+)/ do |url|
	result = system "powershell  " +
						"-ExecutionPolicy Unrestricted " + 
	                    "Import-Module BitsTransfer; " +
	                    "New-Item  c:\\tmp\ -ItemType Directory -ErrorAction SilentlyContinue; " +
	                    "Start-BitsTransfer -Source " + url + " -Destination c:\\tmp\\build_env.ps1; " +
	                    "c:\\tmp\\build_env.ps1 > build_env_output.txt" 
end

Then /I should be able to check out from trunk/ do
	result = system "svn -q checkout http://eid-mw.googlecode.com/svn/trunk/ #{@checkoutdir} "
	result.should == true
end

Then /I should be able to autoreconf/ do
	# by redirecting output to NUL, the console does not crash.
	cmd = "sh -c \"cd \\\"" + @checkoutdir + "\\\";" +
							"autoreconf -i -f;\" > NUL"
	puts cmd
	result = system cmd
	result.should == true
end

Then /I should be able to configure and make for platform (.+)/ do |platform|
	# by redirecting output to NUL, the console does not crash.
	cmd = "sh -c \"cd \\\"" + @checkoutdir + "\\\";" +
							"./configure --host=" + platform + " && make;\" > NUL"
	puts cmd
	result = system cmd
	result.should == true
end
