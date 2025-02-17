import React, { Component } from 'react';
import { isNumber, isFunction } from 'lodash';
import ChartJS from 'chart.js';
import asyncLoader from 'components/asyncLoader';

export const CHART_COLORS = {
    BLUE: '#5086d8',
    YELLOW: '#ecb857',
    GREEN: '#4caf4f',
    RED: '#d25b49'
};

class Chart extends Component {
    componentDidMount() {
        const labels = isFunction(this.props.labels)
            ? this.props.labels(this.props.rawData)
            : this.props.labels;
        let datasets = isFunction(this.props.formatData)
            ? this.props.formatData(this.props.rawData)
            : this.props.datasets;

        datasets = datasets.map(dataset => ({
            label: dataset.label,
            borderColor: dataset.color,
            backgroundColor: dataset.backgroundColor,
            fill: false,
            data: dataset.data,
            lineTension: 0.25
        }));

        const config = {
            type: this.props.type,
            data: {
                labels,
                datasets
            },
            showTooltips: false,
            options: {
                animation: {
                    onComplete: this.props.onAnimationComplete,
                    duration: 2
                },
                responsive: true,
                maintainAspectRatio: false,
                title: {
                    display: true,
                    text: this.props.title
                },
                elements: {
                    point: {
                        radius: this.props.point_radius ? this.props.point_radius : 1
                    }
                },
                scales: {
                    yAxes: [{
                        gridLines: {
                            display: true
                        },
                        scaleLabel : {
                            display: this.props.yLabel != null,
                            labelString: this.props.yLabel
                        },
                    }],
                    xAxes: [{
                        gridLines: {
                            display: false
                        },
                        display: true,
                        scaleLabel: {
                            display: true,
                            labelString: this.props.xLabel
                        },
                        ticks: {
                            display: this.props.displayXTicks != null
                        }
                    }]
                },
                legend: {
                    display: this.props.legend != null,
                    position: 'right',
                    labels: {
                        usePointStyle: true
                    }
                },
                zoom: {
                    enabled: true,
                    drag: true,
                }
            }
        };

        const ctx = this.chart.getContext('2d');
        this.chartInstance = new ChartJS(ctx, config);
    }

    componentWillUnmount() {
        this.chartInstance.destroy();
    }

    onDoubleClick() {
        this.chartInstance.resetZoom();
    }

    onMouseDown(event) {
        if (this.zoomStarted === undefined || this.zoomStarted === null) {
            this.zoomStarted = event.clientX;
        }
    }

    onMouseUp(event) {
        if (this.zoomStarted) {
            const beginPoint = this.zoomStarted;

            const startX = Math.min(beginPoint, event.clientX);
            const endX = Math.max(beginPoint, event.clientX);

            if (startX !== endX) {
                ChartJS.helpers.each(this.chartInstance.scales, (scale, id) => {
                    if (scale.isHorizontal()) {

                        const labels = scale.chart.data.labels;

                        const startValue = scale.getValueForPixel(startX);
                        const endValue = scale.getValueForPixel(endX);
                        const offset = scale.minIndex;

                        scale.options.ticks.min = labels[startValue - offset];
                        scale.options.ticks.max = labels[endValue - offset];
                    }
                });

                this.chartInstance.update();
            }

            this.zoomStarted = null;
        }
    }

    render() {
        const style = {
            height: isNumber(this.props.height) ? `${this.props.height}px` : null
        };

        return (
            <div style={style}>
                <canvas className={`lst-chart-${Date.now()} fade-in`}
                        onDoubleClick={this.onDoubleClick.bind(this)}
                        onMouseDown={this.onMouseDown.bind(this)}
                        onMouseUp={this.onMouseUp.bind(this)}
                        ref={ref => this.chart = ref} />
            </div>
        );
    }
}

export default asyncLoader(Chart, {
    asyncRequests: {
        rawData: props => props.request()
    }
});
