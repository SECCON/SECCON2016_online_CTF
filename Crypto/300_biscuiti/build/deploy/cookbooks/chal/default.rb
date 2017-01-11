file '/var/www/html/index.html' do
  action :delete
end

# Read-only directories
directory "/var/www/db" do
  owner 'root'
  group 'root'
  mode '0755'
  action :create
end

# Read-only files
%W(
/var/www/flag
/var/www/db/users.db
/var/www/html/index.php
/var/www/html/logout.php
).each do |file|
  remote_file file do
    owner 'root'
    group 'root'
    mode '0644'
    action :create
  end
end

