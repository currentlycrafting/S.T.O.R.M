// This file *uses* generated files storm_pb.js and storm_grpc_web_pb.js
// Make sure to run protoc to generate them (command shown below).
import { KeyValueStoreClient } from './proto/storm_grpc_web_pb';
import { GetRequest, PutRequest, DeleteRequest } from './proto/storm_pb';

const client = new KeyValueStoreClient('http://localhost:8080');

export const putKey = (key, value) => {
  return new Promise((resolve, reject) => {
    const req = new PutRequest();
    req.setKey(key);
    req.setValue(value);
    client.put(req, {}, (err, resp) => {
      if (err) reject(err);
      else resolve(resp.getSuccess());
    });
  });
};

export const getKey = (key) => {
  return new Promise((resolve, reject) => {
    const req = new GetRequest();
    req.setKey(key);
    client.get(req, {}, (err, resp) => {
      if (err) reject(err);
      else resolve(resp.getSuccess() ? resp.getValue() : null);
    });
  });
};

export const deleteKey = (key) => {
  return new Promise((resolve, reject) => {
    const req = new DeleteRequest();
    req.setKey(key);
    client.delete(req, {}, (err, resp) => {
      if (err) reject(err);
      else resolve(resp.getSuccess());
    });
  });
};
