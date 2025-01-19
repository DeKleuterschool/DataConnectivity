# Bike Tracker
## Setting up the website
Before setting up the website you need to make sure that you have git and docker installed.  
When you have those installed you can clone the repository by running `git clone https://github.com/DeKleuterschool/DataConnectivity.git`, after running this you should go to the directory by running `cd DataConnectivity`.  
Then you need to setup the configuration for the website, to do this you need to copy `.env.example` to `.env`.  
Make changes to the .env file where necessary.

## Running the website
When the website has been setup you can turn it on by running `docker compose up --build -d`.  
Now you should be able to visit the website by accessing the port that you have setup in the environment file.

## Setting up HTTPS
Another deployment step to make the website secure is setting up HTTPS.  
The way you setup HTTPS depends on your own setup but below you can see the way that we have set everything up.  
First we make sure that nginx is installed on the server.  
We then download certbot for managing our HTTPS certificates.  
```sh
sudo apt update
sudo apt install -y certbot
sudo apt install -y python3-certbot-nginx
```
Then we add an `A` DNS record for the subdomain `dc.slicegames.nl`.  
We can create an SSL certificate for this domain by running `certbot certonly --nginx -d dc.slicegames.nl`.  
After creating the SSL certificate we create the following config file in `/etc/nginx/sites-enabled/dc.conf`.
```
server {
    listen 80;
    server_name dc.slicegames.nl;
    return 301 https://$server_name$request_uri;
}

server {
    listen 443 ssl http2;
    server_name dc.slicegames.nl;
    ssl_certificate /etc/letsencrypt/live/dc.slicegames.nl/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/dc.slicegames.nl/privkey.pem;
    ssl_session_cache shared:SSL:10m;
    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_ciphers "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384";
    ssl_prefer_server_ciphers on;

    location / {
        proxy_pass http://localhost:8000;
    }
}
```
Now that everything is configured we can restart nginx by running `systemctl restart nginx`.  
The website should now be accessible through `https://dc.slicegames.nl`.  
PS: The server that we use for testing this project runs a lot of other services so to save resources the website will most likely be offline.