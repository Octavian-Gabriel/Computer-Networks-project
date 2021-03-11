# Top Music
>Top Music is a client/server application used to manage a top of songs added, voted and commented by users. This application is related to college.
<hr>

# Table of Contents
* [Technologies used](#tech-used)
* [Aoplication capabilities](#project)


# <a name="tech-used"></a> Technologies used
* Linux environment
* C programming language
* TCP/IP communication protocol bewtween clients and server, sockets.
* Multithreading to ensure concurency of the server.
* Sqlite3 database API to store informations about songs and users.
# <a name="project"></a> Application capabilities
Users need to register and log in the application before using it. Each song added has the following informations: band/artist, name of the song,music genre, link, and maybe a description. There a 2 kinds of users: regular and admins.
After a regular user is logged he/she has the following options:
* add a new song
* see the top of all the songs added in the app 
* see the top for a particular genre
* see a list with all the music genres
* vote for a song (increasing its rank).  
* leave a comment for a particular song

An admin has all the actions provided for a regular user and on top of that he can:
* remove a song from the app
* block a user for voting songs. 





