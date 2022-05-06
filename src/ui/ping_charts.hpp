// charts of ping over time (sequence number)

#pragma once

#include <vector>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>


// 
// Base class:
// there could be different chart types, which will be implemented in
// the child classes
// 

class PingChartsBase {

public:
	PingChartsBase(QChartView &view, unsigned npoints)
		: m_npoints(npoints) {
		view.setChart(&m_chart);
	}

	virtual ~PingChartsBase();

	virtual void AppendPoints(const std::vector<QPointF> &);

private:
	unsigned m_npoints;
	QChart m_chart;
};


//
// child class implementing line charts
// 

class PingLineChart : public PingChartsBase {

public:
	PingLineChart();
	virtual ~PingLineChart();

	void AppendPoints(const std::vector<QPointF> &) override;

private:

	QLineSeries m_series;
}
