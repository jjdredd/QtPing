// this is a header for aggregate statistics,
// for mean, std, correlations
// that do not store all the data points
// useful if we have too much data and little memory


#ifndef STATS_HPP
#define STATS_HPP

#include <vector>
#include <string>
#include <cmath>


// most algorithms are from
// 
// Optimization of Pearson correlation coefficient
// calculation for DPA and comparison of different
// approaches
// 
// Petr Socha, Vojtěch Miškovský, Hana Kubátová, Martin Novotný


// mean

template <typename T, typename U = T>
class StatAggregatorMean {

public:
	StatAggregatorMean(U arg)
		: Mean(arg), nSamples(0) {}

	virtual ~StatAggregatorMean() {}


	void ConsumeDataPoint(const T &x) {
		++nSamples;
		Mean = Mean + (x - Mean) / nSamples;
	}

	void Reset() {
		Mean = 0;
		nSamples = 0;
	}

	T CurrentValue() const { return Mean; }
	unsigned CurrentNSamples() const { return nSamples; }

private:
	T Mean;
	unsigned nSamples;
};

// standard deviation
// using Welford's one pass algorithm
// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm

template <typename T, typename U = T>
class StatAggregatorWelfordStD {

public:
	StatAggregatorWelfordStD(U arg)
		: S(arg), Mean(arg) {}

	virtual ~StatAggregatorWelfordStD() {}


	void ConsumeDataPoint(const T &x) {
		T prev_mean = Mean.CurrentValue();
		Mean.ConsumeDataPoint(x);
		T cur_mean = Mean.CurrentValue();
		S = S + (x - prev_mean) * (x - cur_mean);
	}

	void Reset() {
		S = 0;
		Mean.Reset();
	}

	T CurrentValue() const { return sqrt(S / Mean.CurrentNSamples()) ; }
	T CurrentMean() const { return Mean.CurrentValue() ; }
	unsigned CurrentNSamples() const { return Mean.CurrentNSamples(); }

private:
	T S;
	StatAggregatorMean<T, U> Mean;
};

// correlation
// computed from covariance
// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online

template <typename T, typename U = T>
class StatAggregatorCorrelations {

public:
	StatAggregatorCorrelations(U arg)
		: C(arg), yStd(arg), xStd(arg) {}
	virtual ~StatAggregatorCorrelations() {}

	void ConsumeDataPoint(const T &x, const T &y) {
		T yMean = yStd.CurrentMean();
		xStd.ConsumeDataPoint(x);
		yStd.ConsumeDataPoint(y);
		T xMean = xStd.CurrentMean();
		C = C + (x - xMean) * (y - yMean);
	}

	void Reset() {
		C = 0;
		yStd.Reset();
		xStd.Reset();
	}

	T CurrentValue() const { 
		T Cov = C / xStd.CurrentNSamples();
		return Cov / (yStd.CurrentValue() * xStd.CurrentValue());
	}

private:
	T C;
	StatAggregatorWelfordStD<T, U> yStd, xStd;
};


#endif	//
