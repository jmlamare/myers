#include <algorithm>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <myers.h>

using namespace std;

int MyersContext::split(
    size_t begx, size_t endx,
    size_t begy, size_t endy,
    size_t &midxbeg, size_t &midxend,
    size_t &midybeg, size_t &midyend) {
  const size_t lenx = endx - begx;
  const size_t leny = endy - begy;
  const size_t lens = lenx + leny;
  const long begdiff = ((long)begx) - ((long)begy);
  const long enddiff = ((long)endx) - ((long)endy);

  const size_t seshalflen = lens / 2 + lens % 2;
  const size_t sesmaxsize = 2 * min(lenx, leny) + 2;
  forwardx.resize(sesmaxsize, begx);
  reversex.resize(sesmaxsize, endx);
  forwardx[(sesmaxsize + begdiff + 1) % sesmaxsize] = begx;
  reversex[(sesmaxsize + enddiff - 1) % sesmaxsize] = endx;

  long lowdiagf = +((long)sesmaxsize);
  long lowdiagb = +((long)sesmaxsize);
  long uppdiagf = -((long)sesmaxsize);
  long uppdiagb = -((long)sesmaxsize);
  for (size_t seslen = 0; seslen <= seshalflen; seslen++) {
    {
      lowdiagf = begdiff - ((long)seslen);
      uppdiagf = begdiff + ((long)seslen);
      const long mindiag = seslen < leny ? lowdiagf : begdiff - 2 * ((long)leny) + ((long)seslen);
      const long maxdiag = seslen < lenx ? uppdiagf : begdiff + 2 * ((long)lenx) - ((long)seslen);
      for (long diag = mindiag; diag <= maxdiag; diag += 2) {
        const size_t diagid = (sesmaxsize + diag + 0) % sesmaxsize;
        const size_t diagup = (sesmaxsize + diag + 1) % sesmaxsize;
        const size_t diagdw = (sesmaxsize + diag - 1) % sesmaxsize;
        midxbeg = midxend =
            diag == lowdiagf || (diag < uppdiagf && forwardx[diagdw] < forwardx[diagup]) ? forwardx[diagup] : min(endx, forwardx[diagdw] + 1);
        midybeg = midyend = midxbeg - diag;
        while (midxend < endx && midyend < endy && equals(midxend, midyend)) {
          midxend++;
          midyend++;
        }
        forwardx[diagid] = midxend;
        // cout << "f(" << diag << "," << seslen << ")[" << midxbeg << "," << midxend << "][" << midybeg << "," << midyend << "]" << endl;
        if (diag < lowdiagb) {
          continue;
        }
        if (diag > uppdiagb) {
          continue;
        }
        if (lens % 2 == 1 && forwardx[diagid] >= reversex[diagid]) {
          return 2 * seslen - 1;
        }
      }
      {
        lowdiagb = enddiff - ((int)seslen);
        uppdiagb = enddiff + ((int)seslen);
        const long mindiag = seslen < lenx ? lowdiagb : enddiff - 2 * ((long)lenx) + ((long)seslen);
        const long maxdiag = seslen < leny ? uppdiagb : enddiff + 2 * ((long)leny) - ((long)seslen);
        for (long diag = mindiag; diag <= maxdiag; diag += 2) {
          const size_t diagid = (sesmaxsize + diag + 0) % sesmaxsize;
          const size_t diagup = (sesmaxsize + diag + 1) % sesmaxsize;
          const size_t diagdw = (sesmaxsize + diag - 1) % sesmaxsize;
          midxbeg = midxend =
              diag == uppdiagb || (diag > lowdiagb && reversex[diagup] < reversex[diagdw]) ? reversex[diagdw] : max(begx + 1, reversex[diagup]) - 1;
          midybeg = midyend = midxbeg - diag;

          while (midxbeg > begx && midybeg > begy && equals(midxbeg - 1, midybeg - 1)) {
            midxbeg--;
            midybeg--;
          }
          reversex[diagid] = midxbeg;
          // cout << "r(" << diag << "," << seslen << ")[" << midxbeg << "," << midxend << "][" << midybeg << "," << midyend << "]" << endl;
          if (diag < lowdiagf) {
            continue;
          }
          if (diag > uppdiagf) {
            continue;
          }
          if (lens % 2 == 0 && forwardx[diagid] >= reversex[diagid]) {
            return 2 * seslen;
          }
        }
      }
    }
  }
  return 0;
}

void MyersContext::process(std::deque<Diff> &processing, Diff &head) {
  if (head.begx < head.endx && head.begy < head.endy) {
    size_t midxbeg, midxend;
    size_t midybeg, midyend;
    const unsigned long seslen = split(head.begx, head.endx, head.begy, head.endy, midxbeg, midxend, midybeg, midyend);
    // cout << "[" << begx << "," << endx << "][" << begy << "," << endy << "]=>seslen=" << seslen << ";[" << (midxbeg) << "," << (midxend) << "];[" << (midybeg) << "," << (midyend) << "]" << endl;
    if (seslen > 1 || (midxbeg < midxend && midybeg < midyend)) {
      processing.emplace_front(NONE, midxend, head.endx, midyend, head.endy);
      processing.emplace_front(BOTH, midxbeg, midxend, midybeg, midyend);
      processing.emplace_front(NONE, head.begx, midxbeg, head.begy, midybeg);
    } else if (head.endy - head.begy > head.endx - head.begx) {
      long d = (long)head.endx - (long)head.begx;
      processing.emplace_front(NONE, head.endx, head.endx, head.begy + d, head.endy);
      processing.emplace_front(BOTH, head.begx, head.endx, head.begy, head.begy + d);
    } else if (head.endy - head.begy < head.endx - head.begx) {
      long d = (long)head.endy - (long)head.begy;
      processing.emplace_front(NONE, head.begx + d, head.endx, head.endy, head.endy);
      processing.emplace_front(BOTH, head.begx, head.begx + d, head.begy, head.endy);
    } else {
      processing.emplace_front(BOTH, head.begx, head.endx, head.begy, head.endy);
    }
  } else if (head.endx > head.begx) {
    processing.emplace_front(FILEX, head.begx, head.endx, head.begy, head.endy);
  } else {
    processing.emplace_front(FILEY, head.begx, head.endx, head.begy, head.endy);
  }
}

void MyersContext::diff(std::vector<Diff> &summary, std::deque<Diff> processing) {
  processing.emplace_front(OP::NONE, 0, _lenx, 0, _leny);

  while (!processing.empty()) {
    Diff head = processing.front();
    processing.pop_front();
    if (head.op == OP::NONE) {
      process(processing, head);
    } else if (!summary.empty() && summary.back().op == head.op) {
      summary.back() += head;
    } else if (head.endx > head.begx || head.endy > head.begy) {
      summary.push_back(head);
    }
  }
}

void MyersContext::diffrec(
    std::vector<Diff> &output,
    size_t begx, size_t endx,
    size_t begy, size_t endy) {
  if (begx < endx && begy < endy) {
    size_t midxbeg, midxend;
    size_t midybeg, midyend;
    const unsigned long seslen = split(begx, endx, begy, endy, midxbeg, midxend, midybeg, midyend);
    // cout << "[" << begx << "," << endx << "][" << begy << "," << endy << "]=>seslen=" << seslen << ";[" << (midxbeg) << "," << (midxend) << "];[" << (midybeg) << "," << (midyend) << "]" << endl;
    if (seslen > 1 || (midxbeg < midxend && midybeg < midyend)) {
      diffrec(output, begx, midxbeg, begy, midybeg);
      if (midxbeg < midxend && midybeg < midyend) {
        output.emplace_back(BOTH, midxbeg, midxend, midybeg, midyend);
      }
      diffrec(output, midxend, endx, midyend, endy);
    } else if (endy - begy > endx - begx) {
      long d = (long)endx - (long)begx;
      if (d > 0)
        output.emplace_back(BOTH, begx, endx, begy, begy + d);
      diffrec(output, endx, endx, begy + d, endy);
    } else if (endy - begy < endx - begx) {
      long d = (long)endy - (long)begy;
      if (d > 0)
        output.emplace_back(BOTH, begx, begx + d, begy, endy);
      diffrec(output, begx + d, endx, endy, endy);
    } else {
      output.emplace_back(BOTH, begx, endx, begy, endy);
    }
  } else if (endx > begx) {
    output.emplace_back(FILEX, begx, endx, begy, endy);
  } else {
    output.emplace_back(FILEY, begx, endx, begy, endy);
  }
}

void MyersContext::diffrec(std::vector<Diff> &summary) {
  std::vector<Diff> output;
  diffrec(output, 0, _lenx, 0, _leny);

  for (size_t i = 0, j = 0; i < output.size(); i = j + 1) {
    j = i;
    while (j + 1 < output.size() && output[i].op == output[j + 1].op) {
      j++;
    }
    summary.emplace_back(output[i].op, output[i].begx, output[j].endx, output[i].begy, output[j].endy);
    // cout << "OP" << output[i].op << ";[" << output[i].begx << "," << output[j].endx << "][" << output[i].begy << "," << output[j].endy << "]" << endl;
  }
}

void diff(
    std::vector<Diff> &summary,
    std::vector<std::string> &filex,
    std::vector<std::string> &filey) {

  class MyersStringsCtx : public MyersContext {
    std::vector<std::string> &_filex;
    std::vector<std::string> &_filey;

  public:
    MyersStringsCtx(std::vector<std::string> &filex,
                    std::vector<std::string> &filey) : MyersContext(filex.size(), filey.size()), _filex(filex), _filey(filey) {
      // NOP
    }
    bool equals(const size_t posx, const size_t posy) {
      // cout << "eq(" << _filex[posx] << "," << _filey[posy] << ")=" << (_filex[posx] == _filey[posy]) << endl;
      return _filex[posx] == _filey[posy];
    }
  };

  MyersStringsCtx(filex, filey).diff(summary);
}
