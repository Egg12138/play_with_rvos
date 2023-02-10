use std::error::Error;
type Prog = u32;
type FatResult<T> = Result<T, Box<dyn Error>>;
pub(crate) fn get_args() -> FatResult<Prog>{
        todo!()
}


pub(crate) fn run(prog: Prog) -> FatResult<()>{
        todo!()

}