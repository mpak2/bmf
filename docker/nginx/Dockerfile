FROM ubuntu:14.04
MAINTAINER Eranga Bandara (erangaeb@gmail.com)
# install nginx
RUN apt-get update -y
RUN apt-get install -y python-software-properties
RUN apt-get install -y software-properties-common
RUN add-apt-repository -y ppa:nginx/stable
RUN apt-get update -y
RUN apt-get install -y nginx
RUN apt-get install -y vim
# deamon mode off
RUN echo "\ndaemon off;" >> /etc/nginx/nginx.conf
RUN chown -R www-data:www-data /var/lib/nginx
# volume
#VOLUME ["conf.d/биморф.рф.conf"]
# expose ports
EXPOSE 80 443
# add nginx conf
ADD ./conf.d/*.conf /etc/nginx/conf.d/
WORKDIR /etc/nginx
CMD ["nginx"]
