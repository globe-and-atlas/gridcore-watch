#include <pebble.h>

#define PERSIST_LOOK 1
#define PERSIST_DATE 2
#define PERSIST_MOTION 3
#define PERSIST_BG 4
#define PERSIST_FRONT 5
#define PERSIST_DEPTH 6
#define PERSIST_GRID 7

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
static int s_phase = 4;
static int s_tilt = 0;
static bool s_raise_armed;

static int clamp(int value, int lo, int hi) {
  return value < lo ? lo : (value > hi ? hi : value);
}

static Palette palette(void) {
  int bg=0x0A1020, front=0xF4F4DB, depth=0x547BD4;
  int middle=0x49B8D0, accent=0x9BD47A, grid=0x273759;
  if (s_look == 1) {
    bg=0x08151A; front=0xF5F2DF; depth=0x5CD3DE;
    middle=0x176277; accent=0xE9AC67; grid=0x1B3A42;
  } else if (s_look == 2) {
    bg=0x14100D; front=0xF3DFB5; depth=0xD48046;
    middle=0x6F3D24; accent=0xE0A966; grid=0x392B23;
  } else if (s_look == 3) {
    bg=0xE5E8DF; front=0x143948; depth=0x47798E;
    middle=0xB7D1D2; accent=0xAA633F; grid=0xB8C9C9;
  } else if (s_look == 4) {
    bg=0x141719; front=0xF4F3ED; depth=0xA9B3B6;
    middle=0x566164; accent=0xF0EFE8; grid=0x3A4144;
  } else if (s_look == 5) {
    bg=s_bg; front=s_front; depth=s_depth;
    middle=s_depth; accent=s_front; grid=s_grid;
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

static void grid(GContext *ctx, GRect bounds, Palette p) {
  for (int x=12; x<=188; x+=16)
    line(ctx,bounds,x,14,x,214,p.grid,1);
  for (int y=14; y<=214; y+=16)
    line(ctx,bounds,12,y,188,y,p.grid,1);
  for (int i=0;i<2;i++) for (int j=0;j<2;j++) {
    int x=i?186:14, y=j?210:18;
    int dx=i?-10:10, dy=j?-10:10;
    line(ctx,bounds,x,y,x+dx,y,p.accent,2);
    line(ctx,bounds,x,y,x,y+dy,p.accent,2);
  }
}

static void prism(GContext *ctx, GRect bounds, Rect r, Palette p) {
  int base=s_look==2?-7:9;
  int dx=(base*s_phase)/4 + s_tilt;
  int dy=((s_look==2?7:-9)*s_phase)/4;
  Rect rear={r.x+dx,r.y+dy,r.w,r.h};
  if (s_look==0) {
    // Three structural slices behind the fixed, high-contrast front.
    GColor slices[3]={p.accent,p.middle,p.depth};
    for (int i=1;i<=3;i++) {
      Rect layer={r.x+dx*i/3,r.y+dy*i/3,r.w,r.h};
      outline(ctx,bounds,layer,slices[i-1],1);
    }
    int x[4]={r.x,r.x+r.w,r.x+r.w,r.x};
    int y[4]={r.y,r.y,r.y+r.h,r.y+r.h};
    for (int i=0;i<4;i++)
      line(ctx,bounds,x[i],y[i],x[i]+dx,y[i]+dy,p.middle,1);
  } else {
    fill_rect(ctx,bounds,rear,p.middle);
    outline(ctx,bounds,rear,p.depth,1);
    fill_quad(ctx,bounds,r.x,r.y,r.x+r.w,r.y,
              r.x+r.w+dx,r.y+dy,r.x+dx,r.y+dy,p.middle);
    fill_quad(ctx,bounds,r.x+r.w,r.y,r.x+r.w,r.y+r.h,
              r.x+r.w+dx,r.y+r.h+dy,r.x+r.w+dx,r.y+dy,p.depth);
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
  fill_rect(ctx,bounds,r,p.bg);
  outline(ctx,bounds,r,p.front,2);
  if (s_look==1 || s_look==3 || s_look==4) {
    if (r.w>r.h) {
      for (int x=r.x+8;x<r.x+r.w-4;x+=8)
        line(ctx,bounds,x,r.y+1,x,r.y+r.h-1,p.depth,1);
    } else {
      for (int y=r.y+8;y<r.y+r.h-4;y+=8)
        line(ctx,bounds,r.x+1,y,r.x+r.w-1,y,p.depth,1);
    }
  }
}

static void digit(GContext *ctx,GRect bounds,int value,int x,int y,int w,int h) {
  static const uint8_t bits[10]={63,6,91,79,102,109,125,7,127,111};
  if (value<0 || value>9) return;
  Palette p=palette();
  int t=w>=36?8:7;
  Rect seg[7]={
    {x+t/2,y,w-t,t},
    {x+w-t,y+t/2,t,h/2-t/2},
    {x+w-t,y+h/2,t,h/2-t/2},
    {x+t/2,y+h-t,w-t,t},
    {x,y+h/2,t,h/2-t/2},
    {x,y+t/2,t,h/2-t/2},
    {x+t/2,y+h/2-t/2,w-t,t}
  };
  for (int i=0;i<7;i++) if (bits[value]&(1<<i)) prism(ctx,bounds,seg[i],p);
}

static void draw_time(GContext *ctx,GRect bounds,struct tm *local) {
  int hour=local->tm_hour;
  if (!clock_is_24h_style()) hour=hour%12?hour%12:12;
  int two_hours=hour>=10;
  int w=two_hours?31:39;
  int gap=two_hours?6:8;
  int colon_gap=two_hours?12:14;
  int total=(two_hours?2:1)*w+2*w+gap*(two_hours?2:1)+colon_gap;
  int x=(200-total)/2;
  int y=62;
  int h=94;
  if (two_hours) { digit(ctx,bounds,hour/10,x,y,w,h); x+=w+gap; }
  digit(ctx,bounds,hour%10,x,y,w,h);
  x+=w;
  Palette p=palette();
  Rect upper={x+colon_gap/2-3,y+31,6,6};
  Rect lower={x+colon_gap/2-3,y+60,6,6};
  prism(ctx,bounds,upper,p); prism(ctx,bounds,lower,p);
  x+=colon_gap;
  digit(ctx,bounds,local->tm_min/10,x,y,w,h);
  x+=w+gap;
  digit(ctx,bounds,local->tm_min%10,x,y,w,h);
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
  Palette p=palette();
  graphics_context_set_fill_color(ctx,p.bg);
  graphics_fill_rect(ctx,bounds,0,GCornerNone);
  grid(ctx,bounds,p);
  time_t now=time(NULL);
  struct tm *local=localtime(&now);
#ifdef GRIDCORE_FIXTURE
  struct tm fixture=*local;
  fixture.tm_hour=GRIDCORE_HOUR;
  fixture.tm_min=GRIDCORE_MINUTE;
  local=&fixture;
  s_look=GRIDCORE_LOOK;
  s_date=GRIDCORE_DATE;
  p=palette();
#endif
  draw_time(ctx,bounds,local);
  draw_date(ctx,bounds,local,p);
}

static void reveal_tick(void *context) {
  (void)context;
  s_reveal_timer=NULL;
  if (s_phase<4) {
    s_phase++;
    layer_mark_dirty(s_layer);
    if (s_phase<4) s_reveal_timer=app_timer_register(180,reveal_tick,NULL);
    else s_tilt=0;
  }
}

static void start_reveal(int tilt) {
  if (!s_motion) return;
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
  for (uint32_t i=0;i<count;i++) {
    if (data[i].did_vibrate) continue;
    if (data[i].z>=-250) s_raise_armed=true;
    else if (data[i].z<=-600 && s_raise_armed) {
      s_raise_armed=false;
      start_reveal(data[i].x/350);
    }
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
  Tuple *motion=dict_find(iter,MESSAGE_KEY_SETTING_MOTION);
  if (motion && (motion->value->int32==0 || motion->value->int32==1)) {
    s_motion=motion->value->int32; persist_write_int(PERSIST_MOTION,s_motion);
  }
  read_color(iter,MESSAGE_KEY_SETTING_BG,&s_bg,PERSIST_BG);
  read_color(iter,MESSAGE_KEY_SETTING_FRONT,&s_front,PERSIST_FRONT);
  read_color(iter,MESSAGE_KEY_SETTING_DEPTH,&s_depth,PERSIST_DEPTH);
  read_color(iter,MESSAGE_KEY_SETTING_GRID,&s_grid,PERSIST_GRID);
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
  s_look=clamp(s_look,0,5);
  s_date=clamp(s_date,0,3);
  s_motion=clamp(s_motion,0,1);
  s_window=window_create();
  window_set_window_handlers(s_window,(WindowHandlers){.load=load,.unload=unload});
  window_stack_push(s_window,true);
  tick_timer_service_subscribe(MINUTE_UNIT,tick);
  accel_tap_service_subscribe(tap_handler);
  accel_data_service_subscribe(10,accel_handler);
  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
  app_message_register_inbox_received(inbox);
  app_message_open(128,64);
  app_event_loop();
  if (s_reveal_timer) app_timer_cancel(s_reveal_timer);
  app_message_deregister_callbacks();
  accel_data_service_unsubscribe();
  accel_tap_service_unsubscribe();
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}
