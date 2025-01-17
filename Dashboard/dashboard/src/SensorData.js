import React, { useEffect, useState } from 'react';
import axios from 'axios';

import CustomLineChart from './CustomLineChart';

import AirIcon from '@mui/icons-material/Air';
import GrassIcon from '@mui/icons-material/Grass';
import WaterDropIcon from '@mui/icons-material/WaterDrop';
import GrainIcon from '@mui/icons-material/Grain';
import LightModeIcon from '@mui/icons-material/LightMode';


const SensorData = () => {
    const [data, setData] = useState({air_temp: [], air_humid: [], soil_temp: [], soil_moist: [], light: [] });
    useEffect(() => {
        const fetchData = async () => {
            try {
                const response = await axios.get('http://172.20.10.2:5000/data');
                setData(prevData => ({
                    air_temp: [...prevData.air_temp, response.data.air_temp],
                    air_humid: [...prevData.air_humid, response.data.air_humid],
                    soil_temp: [...prevData.soil_temp, response.data.soil_temp],
                    soil_moist: [...prevData.soil_moist, response.data.soil_moist],
                    light: [...prevData.light, response.data.light]
                }));

            } catch (error) {
                console.error('Error fetching data:', error);
            }
        };

        fetchData();
        const interval = setInterval(fetchData, 1000);  

        return () => clearInterval(interval);  
    }, []);
    
    return (
        <div className="content-container">
            <div className="header-container">
                <h1>Sensor Data</h1>
            </div>
            
            <div className="data-container">
                <div className='data-container-inner'>
                    <div className="data-wrapper">
                        <div className="data-icon-container"><AirIcon fontSize="large" /></div>
                        <p>
                            Air Temp.: {Math.round(data.air_temp.at(-1) * 10) / 10} °C
                        </p>
                    </div>
                    <div className="data-wrapper">
                        <div className="data-icon-container"><GrassIcon fontSize="large" /></div>
                        <p>
                            Air Humidity: {Math.round(data.air_humid.at(-1) * 10) / 10}
                        </p>
                    </div>
                    <div className="data-wrapper">
                        <div className="data-icon-container"><WaterDropIcon fontSize="large"/></div>
                        <p>
                            Soil Temp.: {Math.round(data.soil_temp.at(-1) * 10) / 10} °C
                        </p>
                    </div>
                    <div className="data-wrapper">
                        <div className="data-icon-container"><GrainIcon fontSize="large" /></div>
                        <p>
                            Soil Moisture: {Math.round(data.soil_moist.at(-1) * 10) / 10}
                        </p>
                    </div>
                    <div className="data-wrapper">
                        <div className="data-icon-container"><LightModeIcon fontSize="large" /></div>
                        <p>
                            Light: {Math.round(data.light.at(-1) * 10) / 10}
                        </p>
                    </div>
                </div>
            </div>
                
            <div className="chart-container">
                <CustomLineChart
                    series={[
                        { data: data.air_temp, label: 'Air Temperature', showMark: false },
                        { data: data.soil_temp, label: 'Soil Temperature', showMark: false }
                    ]}
                    colors={['#7dabfa', '#faad7d']}
                />
                <CustomLineChart
                    series={[
                        { data: data.light, label: 'Light', showMark: false },
                    ]}
                    colors={['#fae67d']}
                />
                <CustomLineChart
                    series={[
                        { data: data.air_humid, label: 'Air Humidity', showMark: false },
                    ]}
                    colors={['#7dabfa']}
                />
                <CustomLineChart
                    series={[
                        { data: data.soil_moist, label: 'Soil Moisture', showMark: false },
                    ]}
                    colors={['#faad7d']}
                />
            </div>            
            
            

        </div>
    );
};

export default SensorData;