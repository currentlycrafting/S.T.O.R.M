import React from 'react';
import './Node.css';

const Node = ({ store }) => {
    return (
        <div className="node-container">
            <h3>S.T.O.R.M. Node</h3>
            {Object.keys(store).length === 0 ? (
                <p>Store is empty.</p>
            ) : (
                <table className="kv-table">
                    <thead>
                        <tr>
                            <th>Key</th>
                            <th>Value</th>
                        </tr>
                    </thead>
                    <tbody>
                        {Object.entries(store).map(([key, value]) => (
                            <tr key={key}>
                                <td>{key}</td>
                                <td>{value}</td>
                            </tr>
                        ))}
                    </tbody>
                </table>
            )}
        </div>
    );
};

export default Node;
