import React, { Component } from 'react';
import Panel from 'components/common/Panel';
import api from 'utils/api';
import chartFormatters from 'utils/chartFormatters';
import SectionHeader from 'components/common/SectionHeader';
import Chart from 'components/Chart';

class PcapPage extends Component {

    render() {
        const { pcapID } = this.props.match.params;

        return (
            <Panel className="col-xs-12">
                <div className="row">
                    <div className="col-xs-12 col-md-12">
                        <Chart
                            type="line"
                            request={() => api.getPtpOffset(pcapID)}
                            labels={chartFormatters.getTimeLineLabel}
                            formatData={chartFormatters.singleValueChart}
                            xLabel=""
                            title="PTP Offset"
                            height={300}
                            yLabel="ns"
                            point_radius="5"
                        />
                    </div>
                </div>
            </Panel>
        );
    }
}

export default PcapPage;
