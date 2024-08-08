# Webserv `110/125`

<p align="center">
  <img src="https://github.com/pin3dev/42_Cursus/blob/192db266f898636eefa274ed3be4aa8cc11b1397/assets/Webserv/Rdm/webserv_demo.gif" width="600" height="375" />
</p>

## Table of Contents  
- [Project Overview](#overview)  
- [Mandatory Features](#features)  
- [Project Compilation and Execution](#compilation-and-execution)  
- [Tests -  Usage Examples](#usage-examples)  
- [Contributors and License](#contributors)  


## Overview  

The `Webserv` project is designed to handle HTTP requests and responses, implementing basic functionalities of a web server 
such as parsing HTTP requests, managing server configurations, handling connections, and serving static files or CGI scripts.  

## Features  
1. HTTP/1.1 compliant
2. Support for GET, POST, and DELETE methods
3. Custom configuration file support
4. Virtual hosting
5. Autoindexing  
6. Error handling and custom error pages  
7. CGI script execution
 
## Compilation and Execution
To compile the project, clone the repository and use the provided Makefile.  

```bash
git clone https://github.com/pin3dev/42_Webserv.git   
cd 42_Webserv  
make
```

## Usage Examples
If no configuration file is specified, the server will use the default configuration located at configs/default.conf.
```bash
./webserv [configuration file]
```

### Configuration
The configuration file allows you to set up the server and define its behavior. Below is an example of a basic configuration:

```bash

server {
	server_name www.site1;
	listen 8080;
	host localhost;
	root www/site1;
	index index.html;
	client_max_body_size 2M;
	error_page 404.html;

	location / {
		allow_methods GET;
		#try_file tutorial.html;
	}

	location /upload {
		allow_methods GET DELETE;
	}

	location .py {
		allow_methods GET POST;
		cgi_path /cgi;
		cgi_ext .py;
		upload_to /upload;
	}

	location /favicon.ico {
		allow_methods GET;
	}

	location /assets {
		allow_methods GET;
		autoindex on;
	}

	location /redirect {
		allow_methods GET;
		return /;
	}
}
```
#### Configuration Directives
- listen: The port on which the server will listen.
- server_name: The server's domain name.
- root: The root directory for the server.
- index: The default index file.
- error_page: Custom error pages.
- location: Define specific behavior for certain URL patterns.
- autoindex: Enable directory listing.
- cgi_path: Path to the CGI executable.
- cgi_ext: File extension for CGI scripts.
- allow_methods: HTTP methods allowed for the location.
- upload_to: Directory for file uploads.
  
### Code Structure
`Makefile`: Instructions for compiling the project.  
`exe/`: Source files for the main server functionality.  
`inc/`: Header files.  
`configs/`: Example configuration files.  

### Testing
There is a tutorial page in HTML `www/site1/tutorial.html` that helps with testing the server using `curl` and directly in the browser. 
This page provides examples and instructions on how to perform various HTTP requests and view the results.  

To access the tutorial page:

1. Ensure the server is running.
2. Open a web browser and navigate to `http://localhost/tutorial.html`
3. This page will guide you through testing different endpoints and methods supported by the server.

## Contributors

* Ívany Pinheiro aka [`@pin3dev`](https://github.com/pin3dev)  
* Clara Franco aka [`@clima-fr`](https://github.com/clima-fr)  
Feel free to submit issues or pull requests if you have suggestions or improvements.

## License
This project is open-source and available under the MIT License.
