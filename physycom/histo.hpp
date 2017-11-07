/* Copyright 2017 - Alessandro Fabbri */

// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt


#ifndef PHYSYCOM_UTILS_HISTO_HPP
#define PHYSYCOM_UTILS_HISTO_HPP

#include <cmath>
#include <vector>
#include <map>
#include <fstream>

namespace physycom
{
  template<typename T>
  struct histo
  {
    std::map<std::string, std::vector<T>> data;
    std::map<std::string, std::vector<int>> counter;
    T min, max, binw;
    int nmin = 100000, nmax = 0;
    int nbin;

    histo() {};

    histo(T min_, T max_, T binw_)
    {
      min = min_;
      max = max_;
      binw = binw_;
      nbin = int((max - min) / binw);
    }

    // this won't compile with int data
    // but it's handy for floating point
    histo(T min_, T max_, int nbin_)
    {
      min = min_;
      max = max_;
      nbin = nbin_;
      binw = (max - min) / nbin;
    }

    void populate()
    {
      for (auto label : data)
      {
        counter[label.first].resize(nbin, 0);
        for (const auto &d : label.second)
        {
          int idx = int((d - min) / binw);
          if( idx < 0 ) continue;
          counter[label.first][(idx > nbin - 1) ? nbin - 1 : idx]++;
        }

        for(auto c : counter[label.first]) 
        {
          nmin = ( nmin < c ) ? nmin : c;
          nmax = ( nmax > c ) ? nmax : c;
        }
      }
    }

    void dump(const std::string &filename)
    {
      std::ofstream outhisto(filename);
      outhisto << "  bin  ";
      for (const auto &label : counter) outhisto << label.first << "  ";
      outhisto << std::endl;
      for (int i = 0; i < nbin; ++i)
      {
        outhisto << min + i*binw << "\t";
        for (const auto &label : counter) outhisto << counter[label.first][i] << "\t";
        outhisto << std::endl;
      }
      outhisto.close();
    }

    void gnuplot(const std::string &filename) const
    {
      std::string basename = filename.substr(0, filename.find_last_of("."));
      std::ofstream outplt(filename);
      outplt << R"(set terminal pngcairo  transparent enhanced font "Verdana,20" fontscale 0.8 size 960, 720 background rgb 'white'
set output ')" << basename << R"(.png'
# Border xy
set style line 101 lc rgb '#808080' lt 1 lw 1
set border 3 front ls 101
set tics nomirror out scale 0.75
set format '%.0s %c'
set border linewidth 1.5
# Styles
linew = 1.2
set style line 11 lc rgb '#0072bd' linetype 1 linewidth linew # blue
set style line 12 lc rgb '#d95319' linetype 1 linewidth linew # orange
set style line 13 lc rgb '#edb120' linetype 1 linewidth linew # yellow
set style line 14 lc rgb '#7e2f8e' linetype 1 linewidth linew # purple
set style line 15 lc rgb '#77ac30' linetype 1 linewidth linew # green
set style line 16 lc rgb '#4dbeee' linetype 2 linewidth linew # light - blue
set style line 17 lc rgb '#a2142f' linetype 1 linewidth linew # red
set style line 21 lc rgb '#a2142f' pointtype 7 linewidth 0.2 # red
# Grid and Ticks
set ytics 0, )" << int(1.2 * nmax/5) << ", " << 10*nmax << R"( nomirror out scale 0.75
set style line 102 lc rgb '#d6d7d9' lt 1 lw 1
set grid xtics ytics back ls 102
# More options
set style fill solid 1.00 border lt - 1
set style histogram clustered gap 1 title textcolor lt - 1
set style data histograms
set key top center autotitle columnhead
set xrange[-1:)" << nbin << R"(]
set yrange[)" << nmin << ":" << 1.2 * nmax << R"(]
set title 'Title'
set xlabel 'Xlabel'
set ylabel 'Counter'
set xtics border in scale 0, 0 nomirror rotate by - 45
plot ')" << basename << R"(.txt')";

      int cnt = 0, column = 2, style = 11;
      for (const auto &p : counter)
      {
        outplt << ((cnt == 0) ? "" : "     ''") << " using " << column++ << ":xtic(int($0) % 2 == 0 ? stringcolumn(1) : '') title '" << p.first << "' ls " << style++ << ((cnt != counter.size() - 1) ? " ,\\" : "") << std::endl;
        ++cnt;
      }
      outplt.close();
    }

  };

  template<typename T> 
  struct multihisto
  {
    std::map<std::string, histo<T>> hs;
    void add_histo(std::string name, T min, T max, T binw) { hs[name] = histo<T>(min, max, binw); }
    void add_histo(std::string name, T min, T max, int nbin) { hs[name] = histo<T>(min, max, nbin); }
    void push(std::string name, std::string tag, T t) { hs[name].data[tag].push_back(t); }
    void populate() { for(auto &h : hs) h.second.populate(); }
    void dump() { for(auto &h : hs) h.second.dump("histo_" + h.first + ".txt"); }
    void gnuplot() { for(auto &h : hs) h.second.gnuplot("histo_" + h.first + ".plt"); }
  };

  template<typename T>
  struct covstats
  {
    multihisto<T> * mh;
    std::map<std::string, std::map<std::string, std::map<std::string, double>>> cov;     // < tag, column, column, value>
    std::map<std::string, std::map<std::string, std::map<std::string, double>>> quad;    // < tag, column, column, value>
    std::map<std::string, std::map<std::string, double>> mean;                           // < tag, column, value>
    std::map<std::string, int> ndata;

    covstats(multihisto<T> &mh_)
    {
      mh = &mh_;
    }

    void populate()
    {
      // sample numbers
      for (auto x : mh->hs)
      {
        for (auto tag : x.second.data)
        {
          ndata[tag.first] = (int)tag.second.size();
          cout << tag.first << " " << ndata[tag.first] << endl;
        }
        break;
      }

      // mean
      for(auto &x : mh->hs)
        for(auto &tag : x.second.data)
          for (int i = 0; i < tag.second.size(); ++i)
            mean[tag.first][x.first] += x.second.data[tag.first][i];

      // quad mean
      for(auto &x : mh->hs)
        for(auto &y : mh->hs)
          for(auto &tag : x.second.data)
            for (int i = 0; i < tag.second.size(); ++i)
            {
              quad[tag.first][x.first][y.first] += x.second.data[tag.first][i] * y.second.data[tag.first][i];
              //cov[tag.first][x.first][y.first] += (x.second.data[tag.first][i] - mean[tag.first][x.first] / ndata[tag.first]) * (y.second.data[tag.first][i] - mean[tag.first][y.first] / ndata[tag.first]) / ndata[tag.first];
            }


      //// covariance
      //for(auto tag : quad)
      //  for(auto i : tag.second)
      //    for(auto j : i.second)
      //      cov[tag.first][i.first][j.first] = 
      //        quad[tag.first][i.first][j.first] / double(ndata[tag.first]) - mean[tag.first][i.first] * mean[tag.first][j.first] / double(ndata[tag.first] * ndata[tag.first]);
    }
  };

}   // end namespace physycom

#endif // PHYSYCOM_UTILS_HISTO_HPP
