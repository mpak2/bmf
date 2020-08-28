#!/bin/bash

sudo docker build -t proxy .;
sudo docker stack deploy --compose-file ../docker-compose.yml swarm
sudo docker service update swarm_nginx;

