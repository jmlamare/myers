#include <deque>
#include <string>
#include <vector>

enum OP {
  BOTH,
  FILEX,
  FILEY,
  NONE
};

struct Diff {
  OP op;
  size_t begx, endx;
  size_t begy, endy;
  Diff() : op(OP::NONE), begx(0), endx(0), begy(0), endy(0) {
    // NOP
  }
  Diff(const Diff &that) : op(that.op), begx(that.begx), endx(that.endx), begy(that.begy), endy(that.endy) {
    // NOP
  }
  Diff(OP _op, size_t _begx, size_t _endx, size_t _begy, size_t _endy) : op(_op), begx(_begx), endx(_endx), begy(_begy), endy(_endy) {
    // NOP
  }
  Diff &operator=(const Diff &that) {
    this->op = that.op;
    this->begx = that.begx;
    this->begy = that.begy;
    this->endx = that.endx;
    this->endy = that.endy;
    return *this;
  }
  Diff &operator+=(const Diff &that) {
    if (that.op == this->op && this->endx == that.begx && this->endy == that.begy) {
      this->endx = that.endx;
      this->endy = that.endy;
    }
    return *this;
  }
};

class MyersContext {
public:
  MyersContext(size_t lenx, size_t leny) : _lenx(lenx), _leny(leny) {
    // NOP
  }

  virtual bool equals(const size_t posx, const size_t posy) = 0;

  int split(
      size_t begx, size_t endx,
      size_t begy, size_t endy,
      size_t &midxbeg, size_t &midxend,
      size_t &midybeg, size_t &midyend);

  void process(std::deque<Diff> &processing, Diff &head);

  void diff(std::vector<Diff> &summary, std::deque<Diff> processing = std::deque<Diff>());

  void diffrec(
      std::vector<Diff> &output,
      size_t begx, size_t endx,
      size_t begy, size_t endy);

  void diffrec(
      std::vector<Diff> &summary);

private:
  const size_t _lenx, _leny;
  std::vector<size_t> forwardx;
  std::vector<size_t> reversex;
};

extern void diff(
    std::vector<Diff> &summary,
    std::vector<std::string> &filex,
    std::vector<std::string> &filey);
