/// # os_try:
/// ### fatee
/// 第一个实现的功能是提供一个file (FAT文件)的库
/// ```shell
/// file ./a.img 
/// ./os_try ./a.img
/// ```
/// 二者功能是相似的
mod fatee;
fn main() {
    if let Err(e) = fatee::get_args().and_then(fatee::run) {
        eprintln!("{}", e);
        std::process::exit(1);
    } 
    println!("Hello, world!");
}



