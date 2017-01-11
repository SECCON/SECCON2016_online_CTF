package 'apache2'
package 'libapache2-mod-php'
package 'php-sqlite3'

file '/etc/apache2/apache2.conf' do
  action :edit
  block do |content|
    content.gsub!("Options Indexes FollowSymLinks", "Options FollowSymLinks")
  end
end

service 'apache2' do
    action :restart
end
