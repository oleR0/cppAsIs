#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

#ifndef TME_DECLSPEC
#define TME_DECLSPEC
#endif

namespace tme {

struct TME_DECLSPEC vec2 {

  vec2();
  vec2(float x, float y);
  float x = 0;
  float y = 0;
};

vec2 TME_DECLSPEC operator+(const vec2 &l, const vec2 &r);

struct TME_DECLSPEC mat3x2 {
  mat3x2();
  static const mat3x2 identity();
  static const mat3x2 scale(const float &scale);
  static const mat3x2 rotation(const float &thetha);
  static const mat3x2 movement(const vec2 &offset);
  static const mat3x2 match_640x480();
  const std::vector<float> get_floats() const;
  vec2 raw[3];
};

vec2 TME_DECLSPEC operator*(const vec2 &v, const mat3x2 &m);
mat3x2 TME_DECLSPEC operator*(const mat3x2 &m1, const mat3x2 &m2);

enum class event {
  left_pressed,
  left_released,
  right_pressed,
  right_released,
  up_pressed,
  up_released,
  down_pressed,
  down_released,
  select_pressed,
  select_released,
  start_pressed,
  start_released,
  button1_pressed,
  button1_released,
  button2_pressed,
  button2_released,
  turn_off
};

std::ostream &TME_DECLSPEC operator<<(std::ostream &stream, const event e);

class engine;

/// return not null on success
engine *TME_DECLSPEC create_engine();
void TME_DECLSPEC destroy_engine(engine *e);

class TME_DECLSPEC color {
public:
  color() : rgba(0xffffffff) {}
  explicit color(std::uint32_t rgba_);
  color(float r, float g, float b, float a);

  float get_r() const;
  float get_g() const;
  float get_b() const;
  float get_a() const;

  void set_r(const float r);
  void set_g(const float g);
  void set_b(const float b);
  void set_a(const float a);

private:
  std::uint32_t rgba = 0;
};

/// vertex with position only
struct TME_DECLSPEC v0 {
  vec2 pos;
};

/// vertex with position and texture coordinate
struct TME_DECLSPEC v1 {
  vec2 pos;
  color c;
};

/// vertex position + color + texture coordinate
struct TME_DECLSPEC v2 {
  vec2 pos;
  vec2 uv;
  color c;
};

/// triangle with positions only
struct TME_DECLSPEC tri0 {
  tri0();
  v0 v[3];
};

/// triangle with positions and color
struct TME_DECLSPEC tri1 {
  tri1();
  v1 v[3];
};

/// triangle with positions color and texture coordinate
struct TME_DECLSPEC tri2 {
  tri2();
  v2 v[3];
};

class TME_DECLSPEC quad {
public:
  quad(const float &);

  void move_up();
  void move_down();
  void move_left();
  void move_right();

  float get_angle() const;
  vec2 get_position() const;
  tri2 *get_tri2s() const;

private:
  tri2 tr[2];

  enum class direction { up, down, left, right };

  direction dir;
  vec2 position;
  float scale;
  float angle;

  void fill_vert_coord();
  void fill_text_coord();
};

std::istream &TME_DECLSPEC operator>>(std::istream &is, mat3x2 &);
std::istream &TME_DECLSPEC operator>>(std::istream &is, vec2 &);
std::istream &TME_DECLSPEC operator>>(std::istream &is, color &);
std::istream &TME_DECLSPEC operator>>(std::istream &is, v0 &);
std::istream &TME_DECLSPEC operator>>(std::istream &is, v1 &);
std::istream &TME_DECLSPEC operator>>(std::istream &is, v2 &);
std::istream &TME_DECLSPEC operator>>(std::istream &is, tri0 &);
std::istream &TME_DECLSPEC operator>>(std::istream &is, tri1 &);
std::istream &TME_DECLSPEC operator>>(std::istream &is, tri2 &);

class TME_DECLSPEC texture {
public:
  virtual ~texture();
  virtual std::uint32_t get_width() const = 0;
  virtual std::uint32_t get_height() const = 0;
};

class TME_DECLSPEC engine {
public:
  virtual ~engine();
  /// create main window
  /// on success return empty string
  virtual std::string initialize(std::string_view config) = 0;
  /// return seconds from initialization
  virtual float get_time_from_init() = 0;
  /// pool event from input queue
  /// return true if more events in queue
  virtual bool read_input(event &e) = 0;
  virtual texture *create_texture(std::string_view path) = 0;
  virtual void destroy_texture(texture *t) = 0;
  virtual void render(const tri0 &, const color &) = 0;
  virtual void render(const tri1 &) = 0;
  virtual void render(const tri2 &, texture *) = 0;
  virtual void render(const tri2 &t, texture *tex, const mat3x2 &m) = 0;
  virtual void render(const tri2 &t, texture *tex, const mat3x2 &m_rotate,
                      const mat3x2 &m_move) = 0;
  virtual void swap_buffers() = 0;
  virtual void uninitialize() = 0;
};

} // end namespace tme
