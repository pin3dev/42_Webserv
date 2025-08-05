<!--# Webserv `110/125`

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


---


-->

<h1 align="center">Webserv</h1>

<p align="center"> 
  <img src="https://img.shields.io/badge/grade-110%2F125-green?style=for-the-badge&logo=42&labelColor=gray"/>
</p>
<p align="center"> 
  <a href="https://github.com/pin3dev/42_Cursus/tree/main/library/">
    <img src="https://img.shields.io/badge/HTTP/1.1-blue?style=for-the-badge"/>
    <img src="https://img.shields.io/badge/CGI_script-blue?style=for-the-badge"/>
    <img src="https://img.shields.io/badge/Socket-blue?style=for-the-badge"/>
    <img src="https://img.shields.io/badge/Multiplexing-blue?style=for-the-badge"/>
    <img src="https://img.shields.io/badge/ConfigParser-blue?style=for-the-badge"/>
    <img src="https://img.shields.io/badge/Autoindexing-blue?style=for-the-badge"/>
    <img src="https://img.shields.io/badge/Virtual_Hosting-blue?style=for-the-badge"/>

  </a>
</p>
<p align="center">
  <img src="https://github.com/pin3dev/42_Cursus/blob/192db266f898636eefa274ed3be4aa8cc11b1397/assets/Webserv/Rdm/webserv_demo.gif" width="600" height="375" />
</p>

<h3>
  <p align="center"> 
    <a href="#introduction">Introduction</a> • 
    <a href="#structure">Structure</a> • 
    <a href="#docs">Docs</a> • 
    <a href="#cloning">Cloning</a> • 
    <a href="#usage">Usage</a> • 
    <a href="#norms">Norms</a> • 
    <a href="#theoretical">Theoretical</a>   
	<a href="#contributors">Contributors</a>
  </p>
</h3>

## 🗣️ Introduction <a id="introduction"></a>

**Webserv** is a lightweight HTTP/1.1 web server built from scratch in C++. It is capable of serving static files, executing CGI scripts, handling custom routes, virtual hosts, and more.

This project aims to deepen understanding of how web servers work under the hood, while applying knowledge of sockets, HTTP protocol, file I/O, and configuration parsing.

## 🧬 Project Structure <a id="structure"></a>

The project is organized as follows:

* `exe/`: Core source files of the server
* `inc/`: Header files
* `configs/`: Example configuration files
* `www/`: Static web content and HTML tutorials

A `Makefile` is provided to facilitate compilation.

## 🗃️ Documentation <a id="docs"></a>

For a detailed breakdown of how the project works, please visit the documentation link below:

<p align="center"> 
  <a href="https://github.com/pin3dev/42_Cursus/wiki/Webserv">
    <img src="https://img.shields.io/badge/Webserv_Docs-lightgreen?style=for-the-badge"/>
  </a>
</p>


## 🫥 Cloning the Repository <a id="cloning"></a>

To clone this repository and compile the project, run the following commands:

```bash
git clone https://github.com/pin3dev/42_Webserv.git
cd 42_Webserv
```
This will download the project to your local machine. Once inside the `webserv` directory, run the provided `Makefile` to compile the project.

## 🕹️ Compilation and Usage <a id="usage"></a>

### Makefile

The `Makefile` includes the following rules:

* `make`: Builds the `webserv` executable
* `make clean`: Removes object files
* `make fclean`: Removes object files and binary
* `make re`: Rebuilds everything from scratch

To compile the project, run:
```bash
make
```
This will generate the executable `webserv`, which can then be launched with a configuration file.

### Basic Usage

1. Start the server:
```bash
./webserv [config_file]
```
> If no config file is specified, it defaults to `configs/default.conf`.

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

* `listen`: Port the server listens on
* `server_name`: Virtual host domain
* `root`: Document root for serving files
* `index`: Default file to serve
* `error_page`: Custom error page
* `location`: Define specific behavior for URI paths
* `autoindex`: Enable directory listing
* `cgi_path` / `cgi_ext`: CGI execution path and file extension
* `allow_methods`: Accepted HTTP methods (GET, POST, DELETE)
* `upload_to`: File upload destination directory

### 🧪 Testing the Server

A tutorial page is available at:

```bash
http://localhost:<port>/tutorial.html
```

This page provides step-by-step instructions for testing routes using `curl`, HTTP clients, or your browser. It includes examples for different methods like GET, POST, and DELETE.


## ⚠️ Norms and Guidelines Disclaimer <a id="norms"></a>

This project strictly follows the [42 Norm](https://github.com/pin3dev/42_Cursus/blob/b9cd0fe844ddb441d0b3efb98abcee92aee49535/assets/General/norme.en.pdf). Some design decisions may seem unusual but are required to comply with school coding standards.


## 📖 Theoretical Background <a id="theoretical"></a>

All supporting material and theory used for this project are documented and available here:

<p align="center"> 
  <a href="https://github.com/pin3dev/42_Cursus/tree/main/library/#05-Webserv">
    <img src="https://img.shields.io/badge/Webserv_Theory-gray?style=for-the-badge"/>
  </a>
</p>

## 👥 Contributors <a id="contributors"></a>

<a href="https://github.com/pin3dev">
  <img src="https://img.shields.io/badge/Ivany_Pinheiro-%40pin3dev-purple?style=for-the-badge"/>  
</a>  
<br>
<a href="https://github.com/clima-fr">
  <img src="https://img.shields.io/badge/Clara_Franco-%40clima--fr-purple?style=for-the-badge"/>  
</a>

Feel free to open issues or contribute improvements via pull requests!
