#include <pebble.h>

#define PERSIST_LOOK 1
#define PERSIST_DATE 2
#define PERSIST_MOTION 3
#define PERSIST_BG 4
#define PERSIST_FRONT 5
#define PERSIST_DEPTH 6
#define PERSIST_GRID 7
#define PERSIST_MODE 8
#define PERSIST_NIGHT_START 9
#define PERSIST_DAY_START 10
#define PERSIST_NIGHT_BG 11
#define PERSIST_NIGHT_FRONT 12
#define PERSIST_NIGHT_DEPTH 13
#define PERSIST_NIGHT_GRID 14
#define PERSIST_DECORATION 15
#define PERSIST_FONT 16
#define PERSIST_BACKGROUND 17

typedef struct { int x, y, w, h; } Rect;
typedef struct {
  GColor bg, front, depth, middle, accent, grid;
} Palette;

static Window *s_window;
static Layer *s_layer;
static AppTimer *s_reveal_timer;
static int s_look = 0;  // Color Slices
static int s_date = 1;  // YYYY-MM-DD
static int s_motion = 1;
static int s_bg = 0x0A1020;
static int s_front = 0xF4F4DB;
static int s_depth = 0x49B8D0;
static int s_grid = 0x273759;
static int s_mode = 2;  // Auto by local watch time
static int s_night_start = 19;
static int s_day_start = 7;
static int s_night_bg = 0x000000;
static int s_night_front = 0xC0C8D0;
static int s_night_depth = 0x336688;
static int s_night_grid = 0x273759;
static int s_decoration = 0;
static int s_font = 0;
static int s_background = 0;  // Blueprint grid
static GFont s_custom_fonts[4];
static int s_phase = 4;
static int s_tilt = 0;
static int s_angle_x;
static int s_angle_y;
static int32_t s_filtered_x;
static int32_t s_filtered_y;
static bool s_filter_ready;
static bool s_raise_armed;
static bool s_accel_subscribed;
static time_t s_last_reveal;

static int clamp(int value, int lo, int hi) {
  return value < lo ? lo : (value > hi ? hi : value);
}

static int mix_hex(int a, int b, int parts_a) {
  int result=0;
  for (int shift=0;shift<=16;shift+=8) {
    int channel=(((a>>shift)&0xFF)*parts_a+((b>>shift)&0xFF)*(4-parts_a))/4;
    result|=channel<<shift;
  }
  return result;
}

static int angle_step(int32_t milligravity, int current) {
  static const int boundaries[6]={-650,-420,-170,170,420,650};
  // Each boundary has 50 mG of hysteresis on both sides.
  while (current<3 && milligravity>=boundaries[current+3]+50) current++;
  while (current>-3 && milligravity<=boundaries[current+2]-50) current--;
  return current;
}

static bool is_night(const struct tm *local) {
  if (s_mode==0) return false;
  if (s_mode==1) return true;
  return local->tm_hour>=s_night_start || local->tm_hour<s_day_start;
}

static Palette palette(bool night) {
  int bg=0x142A45, front=0xF4F4DB, depth=0x547BD4;
  int middle=0x49B8D0, accent=0x9BD47A, grid=0x2A5873;
  if (s_look == 1) {
    bg=0x08151A; front=0xF5F2DF; depth=0x5CD3DE;
    middle=0x176277; accent=0xE9AC67; grid=0x1B3A42;
  } else if (s_look == 2) {
    bg=0x14100D; front=0xF3DFB5; depth=0xD48046;
    middle=0x6F3D24; accent=0xE0A966; grid=0x5A3A1B;
  } else if (s_look == 3) {
    bg=0xE5E8DF; front=0x143948; depth=0x47798E;
    middle=0xB7D1D2; accent=0xAA633F; grid=0xB8C9C9;
  } else if (s_look == 4) {
    bg=0x000000; front=0xFFFFFF; depth=0xAAAAAA;
    middle=0x555555; accent=0xFFFFFF; grid=0x444444;
  } else if (s_look == 5) {
    bg=s_bg; front=s_front; depth=s_depth;
    middle=mix_hex(s_front,s_depth,2);
    accent=mix_hex(s_front,s_depth,3);
    grid=s_grid;
  }
  if (night) {
    if (s_look==0) {
      bg=0x000408; front=0xC4D2D9; depth=0x286082;
      middle=0x327F8B; accent=0x79A883; grid=0x273759;
    } else if (s_look==1) {
      bg=0x000808; front=0xC6DBD8; depth=0x225F66;
      middle=0x174049; accent=0x8A7B55; grid=0x23524D;
    } else if (s_look==2) {
      bg=0x080400; front=0xCCB78F; depth=0x7A4927;
      middle=0x4B2B18; accent=0xA37F4B; grid=0x5A3A1B;
    } else if (s_look==3) {
      bg=0x07131D; front=0xB3C7CD; depth=0x346478;
      middle=0x163A49; accent=0x967754; grid=0x273759;
    } else if (s_look==4) {
      bg=0x000000; front=0xAAAAAA; depth=0x555555;
      middle=0x333333; accent=0xAAAAAA; grid=0x555555;
    } else {
      bg=s_night_bg; front=s_night_front; depth=s_night_depth;
      middle=mix_hex(s_night_front,s_night_depth,2);
      accent=mix_hex(s_night_front,s_night_depth,3);
      grid=s_night_grid;
    }
  }
  return (Palette){GColorFromHEX(bg), GColorFromHEX(front),
                   GColorFromHEX(depth), GColorFromHEX(middle),
                   GColorFromHEX(accent), GColorFromHEX(grid)};
}

static GPoint point(GRect bounds, int x, int y) {
  return GPoint((bounds.size.w-200)/2+x, (bounds.size.h-228)/2+y);
}

static void line(GContext *ctx, GRect bounds, int x0, int y0,
                 int x1, int y1, GColor color, int width) {
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, width);
  graphics_draw_line(ctx, point(bounds,x0,y0), point(bounds,x1,y1));
}

static void outline(GContext *ctx, GRect bounds, Rect r, GColor color, int width) {
  line(ctx,bounds,r.x,r.y,r.x+r.w,r.y,color,width);
  line(ctx,bounds,r.x+r.w,r.y,r.x+r.w,r.y+r.h,color,width);
  line(ctx,bounds,r.x+r.w,r.y+r.h,r.x,r.y+r.h,color,width);
  line(ctx,bounds,r.x,r.y+r.h,r.x,r.y,color,width);
}

static void fill_rect(GContext *ctx, GRect bounds, Rect r, GColor color) {
  GPoint p=point(bounds,r.x,r.y);
  graphics_context_set_fill_color(ctx,color);
  graphics_fill_rect(ctx,GRect(p.x,p.y,r.w,r.h),0,GCornerNone);
}

static void fill_chamfered_segment(GContext *ctx, GRect bounds, Rect r,
                                  GColor color) {
  const int cut=3;
  GPoint points[6];
  if (r.w>=r.h) {
    points[0]=point(bounds,r.x+cut,r.y);
    points[1]=point(bounds,r.x+r.w-cut,r.y);
    points[2]=point(bounds,r.x+r.w,r.y+r.h/2);
    points[3]=point(bounds,r.x+r.w-cut,r.y+r.h);
    points[4]=point(bounds,r.x+cut,r.y+r.h);
    points[5]=point(bounds,r.x,r.y+r.h/2);
  } else {
    points[0]=point(bounds,r.x+r.w/2,r.y);
    points[1]=point(bounds,r.x+r.w,r.y+cut);
    points[2]=point(bounds,r.x+r.w,r.y+r.h-cut);
    points[3]=point(bounds,r.x+r.w/2,r.y+r.h);
    points[4]=point(bounds,r.x,r.y+r.h-cut);
    points[5]=point(bounds,r.x,r.y+cut);
  }
  GPathInfo info={.num_points=6,.points=points};
  GPath *path=gpath_create(&info);
  if (!path) return;
  graphics_context_set_fill_color(ctx,color);
  gpath_draw_filled(ctx,path);
  gpath_destroy(path);
}

static void fill_quad(GContext *ctx, GRect bounds,
                      int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3,
                      GColor color) {
  GPoint points[4]={point(bounds,x0,y0),point(bounds,x1,y1),
                    point(bounds,x2,y2),point(bounds,x3,y3)};
  GPathInfo info={.num_points=4,.points=points};
  GPath *path=gpath_create(&info);
  if (!path) return;
  graphics_context_set_fill_color(ctx,color);
  gpath_draw_filled(ctx,path);
  gpath_destroy(path);
}

static void background_pattern(GContext *ctx, GRect bounds, Palette p) {
  if (s_background==11) return;
  if (s_background<=2) {
    int step=s_background==0?16:(s_background==1?8:32);
    for (int x=12;x<=188;x+=step)
      line(ctx,bounds,x,14,x,214,p.grid,1);
    for (int y=14;y<=214;y+=step)
      line(ctx,bounds,12,y,188,y,p.grid,1);
  } else if (s_background==3 || s_background==4) {
    graphics_context_set_fill_color(ctx,p.grid);
    for (int x=20;x<=180;x+=20) for (int y=24;y<=204;y+=20) {
      if (s_background==3) graphics_fill_circle(ctx,point(bounds,x,y),1);
      else {
        line(ctx,bounds,x-2,y,x+2,y,p.grid,1);
        line(ctx,bounds,x,y-2,x,y+2,p.grid,1);
      }
    }
  } else if (s_background==5 || s_background==6) {
    int step=s_background==5?20:32;
    for (int x=-200;x<=200;x+=step) {
      line(ctx,bounds,x,14,x+200,214,p.grid,1);
      if (s_background==6)
        line(ctx,bounds,x,214,x+200,14,p.grid,1);
    }
  } else if (s_background==7) {
    for (int y=18;y<=210;y+=8)
      line(ctx,bounds,12,y,188,y,p.grid,1);
  } else if (s_background==8) {
    graphics_context_set_stroke_color(ctx,p.grid);
    graphics_context_set_stroke_width(ctx,1);
    GPoint center=point(bounds,100,114);
    for (int radius=24;radius<=120;radius+=24)
      graphics_draw_circle(ctx,center,radius);
  } else if (s_background==9) {
    for (int i=0;i<6;i++)
      outline(ctx,bounds,(Rect){14+12*i,18+12*i,172-24*i,192-24*i},p.grid,1);
  } else if (s_background==10) {
    graphics_context_set_fill_color(ctx,p.grid);
    for (int i=0;i<48;i++) {
      int x=15+(i*73)%170, y=18+(i*47)%190;
      graphics_fill_circle(ctx,point(bounds,x,y),i%11==0?2:1);
    }
  }
}

static void decoration(GContext *ctx, GRect bounds, Palette p) {
  if (s_decoration==7) return;
  if (s_decoration==6) {
    graphics_context_set_stroke_color(ctx,p.accent);
    graphics_context_set_stroke_width(ctx,1);
    if (bounds.size.w==bounds.size.h)
      graphics_draw_circle(ctx,GPoint(bounds.size.w/2,bounds.size.h/2),113);
    else
      outline(ctx,bounds,(Rect){9,11,182,206},p.accent,1);
    return;
  }
  for (int i=0;i<2;i++) for (int j=0;j<2;j++) {
    int x=s_decoration==1?(i?176:24):(i?186:14);
    int y=s_decoration==1?(j?200:28):(j?210:18);
    int dx=i?-1:1, dy=j?-1:1;
    if (s_decoration==3) {
      line(ctx,bounds,x-5,y,x+5,y,p.accent,1);
      line(ctx,bounds,x,y-5,x,y+5,p.accent,1);
    } else if (s_decoration==4) {
      line(ctx,bounds,x,y,x+dx*12,y,p.accent,2);
      line(ctx,bounds,100,j?211:17,100,j?202:26,p.accent,1);
      line(ctx,bounds,i?187:13,114,i?178:22,114,p.accent,1);
    } else if (s_decoration==5) {
      graphics_context_set_fill_color(ctx,p.accent);
      graphics_fill_circle(ctx,point(bounds,x,y),2);
      graphics_fill_circle(ctx,point(bounds,x+dx*9,y),1);
      graphics_fill_circle(ctx,point(bounds,x,y+dy*9),1);
    } else {
      int length=s_decoration==1?6:10;
      line(ctx,bounds,x,y,x+dx*length,y,p.accent,2);
      line(ctx,bounds,x,y,x,y+dy*length,p.accent,2);
      if (s_decoration==2) {
        int inner_x=x+dx*5, inner_y=y+dy*5;
        line(ctx,bounds,inner_x,inner_y,inner_x+dx*length,inner_y,p.accent,1);
        line(ctx,bounds,inner_x,inner_y,inner_x,inner_y+dy*length,p.accent,1);
      }
    }
  }
}

static void prism(GContext *ctx, GRect bounds, Rect r, Palette p,
                  bool front_pass) {
  if (front_pass) {
    if (s_font==1) outline(ctx,bounds,r,p.front,2);
    else fill_chamfered_segment(ctx,bounds,r,p.front);
    if (s_look==1 || s_look==3 || s_look==4) {
      if (r.w>r.h) {
        for (int x=r.x+8;x<r.x+r.w-4;x+=8)
          line(ctx,bounds,x,r.y+1,x,r.y+r.h-1,p.depth,1);
      } else {
        for (int y=r.y+8;y<r.y+r.h-4;y+=8)
          line(ctx,bounds,r.x+1,y,r.x+r.w-1,y,p.depth,1);
      }
    }
    return;
  }
  int base=s_look==2?-7:9;
  int dx=(base*s_phase)/4 + s_tilt + s_angle_x;
  int dy=((s_look==2?7:-9)*s_phase)/4 + s_angle_y;
  Rect rear={r.x+dx,r.y+dy,r.w,r.h};
  GColor top_shades[3]={p.accent,p.middle,p.depth};
  GColor side_shades[3]={p.depth,p.middle,p.accent};
  if (s_angle_y>0) {
    top_shades[0]=p.depth;
    top_shades[2]=p.accent;
  }
  if (s_angle_x<0) {
    side_shades[0]=p.accent;
    side_shades[2]=p.depth;
  }
  if (s_look==0) {
    int ax=s_angle_x<0?-s_angle_x:s_angle_x;
    int ay=s_angle_y<0?-s_angle_y:s_angle_y;
    GColor *slice_shades=ay>ax?top_shades:side_shades;
    for (int i=1;i<=3;i++) {
      Rect slice={r.x+dx*i/3,r.y+dy*i/3,r.w,r.h};
      outline(ctx,bounds,slice,slice_shades[i-1],1);
    }
    line(ctx,bounds,r.x,r.y,r.x+dx,r.y+dy,p.accent,1);
    line(ctx,bounds,r.x+r.w,r.y,r.x+r.w+dx,r.y+dy,p.middle,1);
  } else {
    fill_rect(ctx,bounds,rear,p.middle);
    outline(ctx,bounds,rear,p.depth,1);
    for (int i=0;i<3;i++) {
      int ax=dx*i/3, ay=dy*i/3;
      int bx=dx*(i+1)/3, by=dy*(i+1)/3;
      fill_quad(ctx,bounds,r.x+ax,r.y+ay,r.x+r.w+ax,r.y+ay,
                r.x+r.w+bx,r.y+by,r.x+bx,r.y+by,top_shades[i]);
      fill_quad(ctx,bounds,r.x+r.w+ax,r.y+ay,r.x+r.w+ax,r.y+r.h+ay,
                r.x+r.w+bx,r.y+r.h+by,r.x+r.w+bx,r.y+by,side_shades[i]);
    }
    fill_quad(ctx,bounds,r.x,r.y+r.h,r.x+r.w,r.y+r.h,
              r.x+r.w+dx,r.y+r.h+dy,r.x+dx,r.y+r.h+dy,p.middle);
    fill_quad(ctx,bounds,r.x,r.y,r.x,r.y+r.h,
              r.x+dx,r.y+r.h+dy,r.x+dx,r.y+dy,p.depth);
    for (int i=0;i<4;i++) {
      int x=(i&1)?r.x+r.w:r.x;
      int y=(i&2)?r.y+r.h:r.y;
      line(ctx,bounds,x,y,x+dx,y+dy,p.depth,1);
    }
  }
}

static void digit(GContext *ctx,GRect bounds,int value,int x,int y,int w,int h,
                  Palette p,bool front_pass) {
  static const uint8_t bits[10]={63,6,91,79,102,109,125,7,127,111};
  if (value<0 || value>9) return;
  int t=w>=36?8:7;
  Rect seg[7]={
    {x+t/2,y,w-t,t},
    {x+w-t,y+t+2,t,h/2-t/2-t-4},
    {x+w-t,y+h/2+t/2+2,t,h/2-t/2-t-4},
    {x+t/2,y+h-t,w-t,t},
    {x,y+h/2+t/2+2,t,h/2-t/2-t-4},
    {x,y+t+2,t,h/2-t/2-t-4},
    {x+t/2,y+h/2-t/2,w-t,t}
  };
  for (int i=0;i<7;i++) if (bits[value]&(1<<i))
    prism(ctx,bounds,seg[i],p,front_pass);
}

static void draw_time(GContext *ctx,GRect bounds,struct tm *local,
                      Palette p,bool front_pass) {
  int hour=local->tm_hour;
  if (!clock_is_24h_style()) hour=hour%12?hour%12:12;
  if (s_font>=2) {
    static const char *keys[10]={
      FONT_KEY_LECO_60_BOLD_NUMBERS_AM_PM,
      FONT_KEY_LECO_60_NUMBERS_AM_PM,
      FONT_KEY_LECO_42_NUMBERS,
      FONT_KEY_ROBOTO_BOLD_SUBSET_49,
      FONT_KEY_BITHAM_42_BOLD,
      FONT_KEY_BITHAM_42_LIGHT,
      NULL,
      NULL,
      NULL,
      NULL
    };
    char label[8];
    snprintf(label,sizeof(label),"%d:%02d",hour,local->tm_min);
    GFont font=s_font==8?s_custom_fonts[0]:
               s_font==9?s_custom_fonts[1]:
               s_font==10?s_custom_fonts[2]:
               s_font==11?s_custom_fonts[3]:fonts_get_system_font(keys[s_font-2]);
    int y=(s_font<=3)?72:((s_font>=8)?77:84);
    int dx=(9*s_phase)/4+s_tilt+s_angle_x;
    int dy=(-9*s_phase)/4+s_angle_y;
    GPoint origin=point(bounds,0,y);
    if (front_pass) {
      graphics_context_set_text_color(ctx,p.front);
      graphics_draw_text(ctx,label,font,GRect(origin.x,origin.y,200,96),
                         GTextOverflowModeTrailingEllipsis,GTextAlignmentCenter,NULL);
    } else {
      graphics_context_set_text_color(ctx,s_angle_x<0?p.middle:p.depth);
      graphics_draw_text(ctx,label,font,
                         GRect(origin.x+dx,origin.y+dy,200,96),
                         GTextOverflowModeTrailingEllipsis,GTextAlignmentCenter,NULL);
      graphics_context_set_text_color(ctx,s_angle_x<0?p.depth:p.middle);
      graphics_draw_text(ctx,label,font,
                         GRect(origin.x+dx/2,origin.y+dy/2,200,96),
                         GTextOverflowModeTrailingEllipsis,GTextAlignmentCenter,NULL);
    }
    return;
  }
  int two_hours=hour>=10;
  int w=two_hours?31:39;
  int gap=two_hours?6:8;
  int colon_gap=two_hours?12:14;
  int total=(two_hours?2:1)*w+2*w+gap*(two_hours?2:1)+colon_gap;
  int x=(200-total)/2;
  int y=62;
  int h=94;
  if (two_hours) { digit(ctx,bounds,hour/10,x,y,w,h,p,front_pass); x+=w+gap; }
  digit(ctx,bounds,hour%10,x,y,w,h,p,front_pass);
  x+=w;
  Rect upper={x+colon_gap/2-3,y+31,6,6};
  Rect lower={x+colon_gap/2-3,y+60,6,6};
  prism(ctx,bounds,upper,p,front_pass); prism(ctx,bounds,lower,p,front_pass);
  x+=colon_gap;
  digit(ctx,bounds,local->tm_min/10,x,y,w,h,p,front_pass);
  x+=w+gap;
  digit(ctx,bounds,local->tm_min%10,x,y,w,h,p,front_pass);
}

static void draw_date(GContext *ctx,GRect bounds,struct tm *local,Palette p) {
  if (!s_date) return;
  char date[20];
  const char *format=s_date==1?"%Y-%m-%d":s_date==2?"%m/%d/%Y":"%d/%m/%Y";
  strftime(date,sizeof(date),format,local);
  GPoint top=point(bounds,0,180);
  graphics_context_set_text_color(ctx,p.front);
  graphics_draw_text(ctx,date,fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
                     GRect(top.x,top.y,200,28),GTextOverflowModeWordWrap,
                     GTextAlignmentCenter,NULL);
}

static void redraw(Layer *layer,GContext *ctx) {
  GRect bounds=layer_get_bounds(layer);
  time_t now=time(NULL);
  struct tm *local=localtime(&now);
#ifdef GRIDCORE_FIXTURE
  struct tm fixture=*local;
  fixture.tm_hour=GRIDCORE_HOUR;
  fixture.tm_min=GRIDCORE_MINUTE;
  local=&fixture;
#endif
  Palette p=palette(is_night(local));
  graphics_context_set_fill_color(ctx,p.bg);
  graphics_fill_rect(ctx,bounds,0,GCornerNone);
  background_pattern(ctx,bounds,p);
  decoration(ctx,bounds,p);
  draw_time(ctx,bounds,local,p,false);
  draw_time(ctx,bounds,local,p,true);
  draw_date(ctx,bounds,local,p);
}

static void reveal_tick(void *context) {
  (void)context;
  s_reveal_timer=NULL;
  if (s_phase<4) {
    s_phase++;
    if (s_phase==4) s_tilt=0;
    layer_mark_dirty(s_layer);
    if (s_phase<4) s_reveal_timer=app_timer_register(180,reveal_tick,NULL);
  }
}

static void start_reveal(int tilt) {
  if (!s_motion) return;
  time_t now=time(NULL);
  if (s_last_reveal && now-s_last_reveal<5) return;
  s_last_reveal=now;
  if (s_reveal_timer) app_timer_cancel(s_reveal_timer);
  s_tilt=clamp(tilt,-2,2);
  s_phase=0;
  layer_mark_dirty(s_layer);
  s_reveal_timer=app_timer_register(180,reveal_tick,NULL);
}

static void tap_handler(AccelAxisType axis,int32_t direction) {
  (void)axis;
  start_reveal(direction>0?1:-1);
}

static void accel_handler(AccelData *data,uint32_t count) {
  int32_t sum_x=0, sum_y=0;
  uint32_t samples=0;
  for (uint32_t i=0;i<count;i++) {
    if (data[i].did_vibrate) continue;
    sum_x+=data[i].x;
    sum_y+=data[i].y;
    samples++;
    if (data[i].z>=-250) s_raise_armed=true;
    else if (data[i].z<=-600 && s_raise_armed) {
      s_raise_armed=false;
      start_reveal(data[i].x/350);
    }
  }
  if (!samples) return;
  int32_t avg_x=sum_x/(int32_t)samples;
  int32_t avg_y=sum_y/(int32_t)samples;
  if (!s_filter_ready) {
    s_filtered_x=avg_x;
    s_filtered_y=avg_y;
    s_filter_ready=true;
  } else {
    s_filtered_x=(3*s_filtered_x+avg_x)/4;
    s_filtered_y=(3*s_filtered_y+avg_y)/4;
  }
  int new_x=angle_step(s_filtered_x,s_angle_x);
  int new_y=angle_step(-s_filtered_y,s_angle_y);
  if (new_x!=s_angle_x || new_y!=s_angle_y) {
    s_angle_x=new_x;
    s_angle_y=new_y;
    layer_mark_dirty(s_layer);
  }
}

static void update_motion_subscription(void) {
  if (s_motion && !s_accel_subscribed) {
    s_raise_armed=false;
    s_filter_ready=false;
    accel_tap_service_subscribe(tap_handler);
    accel_data_service_subscribe(5,accel_handler);
    accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
    s_accel_subscribed=true;
  } else if (!s_motion && s_accel_subscribed) {
    accel_data_service_unsubscribe();
    accel_tap_service_unsubscribe();
    s_accel_subscribed=false;
    if (s_reveal_timer) app_timer_cancel(s_reveal_timer);
    s_reveal_timer=NULL;
    s_phase=4;
    s_tilt=0;
    s_angle_x=0;
    s_angle_y=0;
    s_filter_ready=false;
  }
}

static void tick(struct tm *time_now,TimeUnits changed) {
  (void)time_now; (void)changed;
  layer_mark_dirty(s_layer);
}

static void read_color(DictionaryIterator *iter,uint32_t key,int *value,int persist_key) {
  Tuple *tuple=dict_find(iter,key);
  if (tuple && tuple->value->int32>=0 && tuple->value->int32<=0xFFFFFF) {
    *value=tuple->value->int32;
    persist_write_int(persist_key,*value);
  }
}

static void inbox(DictionaryIterator *iter,void *context) {
  (void)context;
  Tuple *look=dict_find(iter,MESSAGE_KEY_SETTING_LOOK);
  if (look && look->value->int32>=0 && look->value->int32<=5) {
    s_look=look->value->int32; persist_write_int(PERSIST_LOOK,s_look);
  }
  Tuple *date=dict_find(iter,MESSAGE_KEY_SETTING_DATE);
  if (date && date->value->int32>=0 && date->value->int32<=3) {
    s_date=date->value->int32; persist_write_int(PERSIST_DATE,s_date);
  }
  Tuple *decoration_value=dict_find(iter,MESSAGE_KEY_SETTING_DECORATION);
  if (decoration_value && decoration_value->value->int32>=0 && decoration_value->value->int32<=7) {
    s_decoration=decoration_value->value->int32;
    persist_write_int(PERSIST_DECORATION,s_decoration);
  }
  Tuple *font_value=dict_find(iter,MESSAGE_KEY_SETTING_FONT);
  if (font_value && font_value->value->int32>=0 && font_value->value->int32<=11) {
    s_font=font_value->value->int32;
    persist_write_int(PERSIST_FONT,s_font);
  }
  Tuple *background_value=dict_find(iter,MESSAGE_KEY_SETTING_BACKGROUND);
  if (background_value && background_value->value->int32>=0 && background_value->value->int32<=11) {
    s_background=background_value->value->int32;
    persist_write_int(PERSIST_BACKGROUND,s_background);
  }
  Tuple *mode=dict_find(iter,MESSAGE_KEY_SETTING_MODE);
  if (mode && mode->value->int32>=0 && mode->value->int32<=2) {
    s_mode=mode->value->int32; persist_write_int(PERSIST_MODE,s_mode);
  }
  Tuple *night_start=dict_find(iter,MESSAGE_KEY_SETTING_NIGHT_START);
  if (night_start && night_start->value->int32>=18 && night_start->value->int32<=22) {
    s_night_start=night_start->value->int32;
    persist_write_int(PERSIST_NIGHT_START,s_night_start);
  }
  Tuple *day_start=dict_find(iter,MESSAGE_KEY_SETTING_DAY_START);
  if (day_start && day_start->value->int32>=5 && day_start->value->int32<=9) {
    s_day_start=day_start->value->int32;
    persist_write_int(PERSIST_DAY_START,s_day_start);
  }
  Tuple *motion=dict_find(iter,MESSAGE_KEY_SETTING_MOTION);
  if (motion && (motion->value->int32==0 || motion->value->int32==1)) {
    s_motion=motion->value->int32; persist_write_int(PERSIST_MOTION,s_motion);
    update_motion_subscription();
  }
  read_color(iter,MESSAGE_KEY_SETTING_BG,&s_bg,PERSIST_BG);
  read_color(iter,MESSAGE_KEY_SETTING_FRONT,&s_front,PERSIST_FRONT);
  read_color(iter,MESSAGE_KEY_SETTING_DEPTH,&s_depth,PERSIST_DEPTH);
  read_color(iter,MESSAGE_KEY_SETTING_GRID,&s_grid,PERSIST_GRID);
  read_color(iter,MESSAGE_KEY_SETTING_NIGHT_BG,&s_night_bg,PERSIST_NIGHT_BG);
  read_color(iter,MESSAGE_KEY_SETTING_NIGHT_FRONT,&s_night_front,PERSIST_NIGHT_FRONT);
  read_color(iter,MESSAGE_KEY_SETTING_NIGHT_DEPTH,&s_night_depth,PERSIST_NIGHT_DEPTH);
  read_color(iter,MESSAGE_KEY_SETTING_NIGHT_GRID,&s_night_grid,PERSIST_NIGHT_GRID);
  layer_mark_dirty(s_layer);
}

static void load(Window *window) {
  s_layer=layer_create(layer_get_bounds(window_get_root_layer(window)));
  layer_set_update_proc(s_layer,redraw);
  layer_add_child(window_get_root_layer(window),s_layer);
}

static void unload(Window *window) {
  (void)window;
  layer_destroy(s_layer);
}

int main(void) {
  if (persist_exists(PERSIST_LOOK)) s_look=persist_read_int(PERSIST_LOOK);
  if (persist_exists(PERSIST_DATE)) s_date=persist_read_int(PERSIST_DATE);
  if (persist_exists(PERSIST_MOTION)) s_motion=persist_read_int(PERSIST_MOTION);
  if (persist_exists(PERSIST_BG)) s_bg=persist_read_int(PERSIST_BG);
  if (persist_exists(PERSIST_FRONT)) s_front=persist_read_int(PERSIST_FRONT);
  if (persist_exists(PERSIST_DEPTH)) s_depth=persist_read_int(PERSIST_DEPTH);
  if (persist_exists(PERSIST_GRID)) s_grid=persist_read_int(PERSIST_GRID);
  if (persist_exists(PERSIST_MODE)) s_mode=persist_read_int(PERSIST_MODE);
  if (persist_exists(PERSIST_NIGHT_START)) s_night_start=persist_read_int(PERSIST_NIGHT_START);
  if (persist_exists(PERSIST_DAY_START)) s_day_start=persist_read_int(PERSIST_DAY_START);
  if (persist_exists(PERSIST_NIGHT_BG)) s_night_bg=persist_read_int(PERSIST_NIGHT_BG);
  if (persist_exists(PERSIST_NIGHT_FRONT)) s_night_front=persist_read_int(PERSIST_NIGHT_FRONT);
  if (persist_exists(PERSIST_NIGHT_DEPTH)) s_night_depth=persist_read_int(PERSIST_NIGHT_DEPTH);
  if (persist_exists(PERSIST_NIGHT_GRID)) s_night_grid=persist_read_int(PERSIST_NIGHT_GRID);
  if (persist_exists(PERSIST_DECORATION)) s_decoration=persist_read_int(PERSIST_DECORATION);
  if (persist_exists(PERSIST_FONT)) s_font=persist_read_int(PERSIST_FONT);
  if (persist_exists(PERSIST_BACKGROUND)) s_background=persist_read_int(PERSIST_BACKGROUND);
  s_look=clamp(s_look,0,5);
  s_date=clamp(s_date,0,3);
  s_motion=clamp(s_motion,0,1);
  s_mode=clamp(s_mode,0,2);
  s_night_start=clamp(s_night_start,18,22);
  s_day_start=clamp(s_day_start,5,9);
  s_decoration=clamp(s_decoration,0,7);
  s_font=clamp(s_font,0,11);
  s_background=clamp(s_background,0,11);
#ifdef GRIDCORE_FIXTURE
  s_look=GRIDCORE_LOOK;
  s_date=GRIDCORE_DATE;
  s_mode=GRIDCORE_MODE;
  s_motion=0;
  s_angle_x=clamp(GRIDCORE_ANGLE_X,-3,3);
  s_angle_y=clamp(GRIDCORE_ANGLE_Y,-3,3);
  s_decoration=clamp(GRIDCORE_DECORATION,0,7);
  s_font=clamp(GRIDCORE_FONT,0,11);
  s_background=clamp(GRIDCORE_BACKGROUND,0,11);
#endif
  s_window=window_create();
  s_custom_fonts[0]=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_CORMORANT_54));
  s_custom_fonts[1]=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BODONI_54));
  s_custom_fonts[2]=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SPACE_GROTESK_54));
  s_custom_fonts[3]=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_GRIDCORE_BOOK_54));
  window_set_window_handlers(s_window,(WindowHandlers){.load=load,.unload=unload});
  window_stack_push(s_window,true);
  tick_timer_service_subscribe(MINUTE_UNIT,tick);
  update_motion_subscription();
  app_message_register_inbox_received(inbox);
  app_message_open(256,64);
  app_event_loop();
  if (s_reveal_timer) app_timer_cancel(s_reveal_timer);
  app_message_deregister_callbacks();
  if (s_accel_subscribed) {
    accel_data_service_unsubscribe();
    accel_tap_service_unsubscribe();
  }
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
  for (int i=0;i<4;i++) fonts_unload_custom_font(s_custom_fonts[i]);
}
