use indicatif::ProgressBar;
use rug::Integer;

fn main() {
    let argv: Vec<String> = std::env::args().collect();
    let t = argv[1].parse::<u64>().unwrap();
    let n = argv[2].parse::<Integer>().unwrap();
    let mut x = Integer::from(2);
    let bar = ProgressBar::new(1000);
    let step = std::cmp::max(t / 1000, 1);
    for i in 0..t {
        if i % step == 0 {
            bar.inc(1);
        }
        x.square_mut();
        x.modulo_mut(&n);
    }
    bar.finish();
    println!("{}", x);
}
