// filepath: /path/to/your/project/sensor-data-app/src/App.js
import React from 'react';
import './App.css';
import SensorData from './components/SensorData';

function App() {
    return (
        <div className="App">
            <header className="App-header">
                <SensorData />
            </header>
        </div>
    );
}

export default App;