// 
// affy/sdk/file/TsvFile/TsvFileDiff.h ---
// 
// $Id: TsvFileDiff.h,v 1.1 2008/05/08 20:52:59 harley Exp $
// 

#include "file/TsvFile/TsvFile.h"
//
#include <string>

namespace affx {
  class TsvFileDiff;
};

//
class affx::TsvFileDiff {
public:
  //
  bool m_opt_print_same;
  bool m_opt_print_linenums;
  int  m_opt_print_format;
  int  m_opt_print_max;
  //
  bool m_opt_do_headers;
  bool m_opt_do_data;
  double m_opt_max_diff;
  //
  int m_diff_cnt;
  int m_print_cnt;

  enum {
    FMT_1 = 1,
    FMT_2 = 2,
  };

  //
  std::string m_residuals_filename;
  affx::TsvFile* m_residuals_tsv;
  
  //
  TsvFileDiff();
  ~TsvFileDiff();

  //
  void clear();
  void open_residuals(const std::string& filename,
                      affx::TsvFile& tsv1,affx::TsvFile& tsv2);
  void close_residuals();
  // 
  void p_linenums(affx::TsvFile& tsv1,affx::TsvFile& tsv2);
  void p_linenums(int lnum1,int lnum2);
  void p_line(affx::TsvFile& tsv);
  bool p_inc();
  void p_diff_1line(affx::TsvFile& tsv1,affx::TsvFile& tsv2);
  void p_diff_2line(affx::TsvFile& tsv1,affx::TsvFile& tsv2);

  //
  int readOptionsFrom(const std::string& filename);
  int readOptionsFrom(affx::TsvFile& tsv);

  //
  int diff(const std::string& filename1,const std::string& filename2);
  int diff(affx::TsvFile& tsv1,affx::TsvFile& tsv2);
  int diffHeaders(affx::TsvFile& tsv1, affx::TsvFile& tsv2);
  int diffData(affx::TsvFile& tsv1, affx::TsvFile& tsv2);
};

