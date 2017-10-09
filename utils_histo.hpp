#ifndef PHYSYCOM_UTILS_HISTO_HPP
#define PHYSYCOM_UTILS_HISTO_HPP

#include <vector>
#include <map>
#include <fstream>

namespace physycom{

  template<typename T>
  struct Histo_data
  {
    std::map<std::string, std::vector<T>> data;
    std::map<std::string, std::vector<int>> counter;
    T min, max, binw;
    int nbin;

    Histo_data() {};

    Histo_data(T min_, T max_, T binw_)
    {
      min = min_;
      max = max_;
      binw = binw_;
      nbin = int((max - min) / binw);
    }

    void populate()
    {
      for (auto label : data)
      {
        counter[label.first].resize(nbin, 0);
        for (const auto &d : label.second)
        {
          int idx = int((d - min) / binw);
          idx = (idx < nbin) ? idx : nbin - 1;
          counter[label.first][idx]++;
        }
      }
    }

    void dump(const std::string &filename)
    {
      std::ofstream outhisto(filename);
      outhisto << "#   bin   #  ";
      for (auto label : counter) outhisto << label.first << "  #  ";
      outhisto << endl;
      for (int i = 0; i < nbin; ++i)
      {
        outhisto << i*binw << "\t";
        for (auto label : counter) outhisto << counter[label.first][i] << "\t";
        outhisto << std::endl;
      }
      outhisto.close();
    }

    void gnuplot(const std::string &filename)
    {
      std::string basename = filename.substr(0, filename.find_last_of("."));
      std::ofstream outplt(filename);
      outplt << R"(set terminal pngcairo  transparent enhanced font "Verdana,10" fontscale 0.8 size 960, 720 background rgb 'white'
set output ')" << basename << R"(.png'
# Border xy
set style line 101 lc rgb '#808080' lt 1 lw 1
set border 3 front ls 101
set tics nomirror out scale 0.75
set format '%g'
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
set ytics 0, 1000, 10000 nomirror out scale 0.75
set style line 102 lc rgb '#d6d7d9' lt 1 lw 1
set grid xtics ytics back ls 102
# More options
set style fill solid 1.00 border lt - 1
set style histogram clustered gap 1 title textcolor lt - 1
set style data histograms
set key top center
set xrange[-1:)" << nbin << R"(]
set yrange[0:*]
set title 'Title'
set xlabel 'Xlabel'
set ylabel 'Ylabel'
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

}   // end namespace physycom

#endif // PHYSYCOM_UTILS_HISTO_HPP
