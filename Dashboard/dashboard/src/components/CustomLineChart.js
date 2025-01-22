import React from 'react';
import { LineChart } from '@mui/x-charts/LineChart';
import { axisClasses } from '@mui/x-charts/ChartsAxis';

const CustomLineChart = ({ series, colors }) => {
    const xAxisData = series[0]?.data.map((_, index) => index) || [];
    return (
        <LineChart
            width={500}
            height={250}
            colors={colors}
            series={series}
            sx={({
                [`.${axisClasses.root}`]: {
                    [`.${axisClasses.tick}, .${axisClasses.line}`]: {
                    stroke: '#ddd',
                    strokeWidth: 1,
                    },
                    [`.${axisClasses.tickLabel}`]: {
                    fill: '#ddd',
                    },
                }})}
            slotProps={{
                legend: {
                    labelStyle: {
                    fontSize: 14,
                    fill: '#ddd',
                    fontWeight: '700'
                    },
                },
            }}
            xAxis={[{
                data: xAxisData,
                disableTicks: false,
            }]}
        />
    );
};

export default CustomLineChart;