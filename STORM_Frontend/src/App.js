import React, { useState } from 'react';
import Node from './Node';
import './App.css';
import { putKey, getKey, deleteKey } from './grpcClient';

function App() {
  const [store, setStore] = useState({});
  const [key, setKey] = useState('');
  const [value, setValue] = useState('');
  const [output, setOutput] = useState('');

  const handlePut = async () => {
    if (!key) { setOutput('Please enter a key.'); return; }
    try {
      const success = await putKey(key, value);
      if (success) {
        setStore(prev => ({ ...prev, [key]: value }));
        setOutput(`PUT successful: ${key}=${value}`);
        setKey('');
        setValue('');
      } else {
        setOutput('PUT failed on server.');
      }
    } catch (err) {
      setOutput('PUT error: ' + err.message);
    }
  };

  const handleGet = async () => {
    if (!key) { setOutput('Please enter a key.'); return; }
    try {
      const result = await getKey(key);
      if (result !== null) setOutput(`GET: ${key}=${result}`);
      else setOutput(`GET failed: Key '${key}' not found.`);
    } catch (err) {
      setOutput('GET error: ' + err.message);
    }
  };

  const handleDelete = async () => {
    if (!key) { setOutput('Please enter a key.'); return; }
    try {
      const success = await deleteKey(key);
      if (success) {
        setStore(prev => { const next = { ...prev }; delete next[key]; return next; });
        setOutput(`DELETE successful: ${key}`);
        setKey('');
      } else {
        setOutput(`DELETE failed: Key '${key}' not found.`);
      }
    } catch (err) {
      setOutput('DELETE error: ' + err.message);
    }
  };

  return (
    <div className="app-container">
      <header>
        <h1>S.T.O.R.M. Key-Value Store</h1>
        <p>Single-node in-memory store (gRPC backend)</p>
      </header>
      <main>
        <div className="controls-container">
          <input type="text" placeholder="Key" value={key} onChange={(e) => setKey(e.target.value)} />
          <input type="text" placeholder="Value (for PUT)" value={value} onChange={(e) => setValue(e.target.value)} />
          <div className="button-group">
            <button onClick={handlePut}>PUT</button>
            <button onClick={handleGet}>GET</button>
            <button onClick={handleDelete}>DELETE</button>
          </div>
        </div>

        <div className="output-container">
          <pre>{output}</pre>
        </div>

        <Node store={store} />
      </main>
    </div>
  );
}

export default App;
