FROM node:18

WORKDIR /app
COPY package.json package-lock.json* /app/
RUN npm install

COPY . /app

# ensure the generated proto JS exists (if you prefer, copy in pre-generated protos)
# If you want to avoid generating inside the container, run protoc locally and copy files into src/proto.
EXPOSE 3000
CMD ["npm", "start"]
