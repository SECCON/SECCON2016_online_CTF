package "python-crypto"

user "alpha-complex-1" do
  home "/home/alpha-complex-1"
  system_user false
end
group "alpha-complex-1"

directory "/home/alpha-complex-1" do
  owner "alpha-complex-1"
  group "alpha-complex-1"
  action :create
  mode "0700"
end

remote_file "/etc/xinetd.d/alpha-complex-1" do
  owner "root"
  group "root"
end

%W(
/home/alpha-complex-1/Complex.py
/home/alpha-complex-1/flag
/home/alpha-complex-1/server.py
).each do |file|
  remote_file file do
    owner "alpha-complex-1"
    group "alpha-complex-1"
    action :create
  end
end

%W(
/home/alpha-complex-1/server.sh
).each do |file|
  remote_file file do
    owner "alpha-complex-1"
    group "alpha-complex-1"
    action :create
    mode "0755"
  end
end

service "xinetd" do
  action :reload
end
