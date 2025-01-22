import React from 'react';
import Button from '@mui/material/Button';


const ExportButton = (data) => {
    const download = () => {
        const { air_temp, air_humid, soil_temp, soil_moist, light } = data.data;


        const output = [
            ["Air temp.", "Air humid.", "Soil temp.", "Soil moist.", "Light"],
            ...air_temp.map((_, i) => [air_temp[i], air_humid[i], soil_temp[i], soil_moist[i], light[i]])
        ]
        .map(row => row.join(";"))
        .join("\n")
        .replaceAll(".", ",");
    
        const blob = new Blob([output], { type: 'text/csv' });
        
        const url = URL.createObjectURL(blob);
        const link = document.createElement('a');
        link.href = url;
        link.download = 'data.csv';
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
        URL.revokeObjectURL(url);
      };

    return (
        <Button onClick={download} variant="contained">Export .csv</Button>
    );
};

export default ExportButton;