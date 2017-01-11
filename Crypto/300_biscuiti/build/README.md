biscuiti
========

Test Deployment and Exploitation
--------------------------------
```
cd deploy
vagrant up
itamae ssh --vagrant roles/main.rb

cd ../../answer
perl solve.pl
```

Deployment
----------
```
cd deploy
itamae ssh --host target.server --user mma roles/main.rb
```

