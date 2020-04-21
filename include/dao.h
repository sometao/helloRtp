#include <string>
#include <memory>

using std::string;
namespace littleGift {
namespace dao {



struct SlidesRow {
  int64_t id = -1;
  string authorName;
  string content;
  string contentType;
  string accessToken;
  string editCode;
  int64_t createTime = -1;
};

int64_t addSlides(const SlidesRow& row);

std::shared_ptr<SlidesRow> getSlides(const string& token);

void initContentTable();

void init();

}  // namespace dao


}  // namespace littleGIft
