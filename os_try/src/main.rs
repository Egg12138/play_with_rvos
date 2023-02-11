#![allow(non_snake_case)]
/// # os_try:
/// ### fatee
/// 第一个实现的功能是提供一个file (FAT文件)的库
/// ```shell
/// file ./a.img 
/// ./os_try ./a.img
/// ```
/// 二者功能是相似的
/// 基本功能：
/// 读取FAT的基本信息 && 当遇到一个Dir时显示它为一个目录 && 不合法格式则提示
mod fatee;
fn main() {
    if let Err(e) = fatee::get_args().and_then(fatee::run) {
        eprintln!("{}", e);
        std::process::exit(1);
    } 
    println!("Hello, world!");
}


#[cfg(test)]
mod tests {
    use crate::fatee::*;
    #[test]
    fn BPB_works0() {
        assert_eq!(std::mem::size_of::<BPB>(), 512);
    }
}

