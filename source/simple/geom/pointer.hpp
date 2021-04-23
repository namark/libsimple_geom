// pointer into 2D span is (ptr, offset) and dimesntion_step common to all pointers of given range
// ptr is a pointer to exact memory location
// offset is the offset from the beginning of the current row
// dimesntion_step is the step to get from the end of row to begin of next row (pitch - width)
//
// begin != end returns a vector<bool,2> (should it be conjunction or disjunction??), indicating in which directions the end has been reached
// (begin.offset != end.offset, begin.ptr != end.ptr)
// (false, false) - we are safely inside the range
// (true, false) - we reached the end of the row
// (true, true) - we reached the end of the range
// (false, true) - not possible, invariant broken
// more complicated with more dimensions, when end is reached not all dimentins maybe true, only the first offset and the ptr
// ooorrrr, might be much simpler if say y oggset represents the number of pixel in current plane instead of y offset, so it'll go till width*height, and also increment with each step, hmmm... then z would be width*height*depth... it could escalate, however it's the same problem as with size_t or ptrdiff_t, the image still needs to fit in memory so these should not overflow... but definitely a potential limitation in more generic context, where image might not be in memory actually... with this we can get mote consistent and meanigful !=, and ++, however strictly speaking we will be doing more operations (simd though... simd can't do 3 integers and a pointer genious -_- ... but but I can dreaaam) aaand other comparison operators would also wooork, so cool
//
// end - begin returns a vector<ptrdiff_t, 2>
// (end.ptr - begin.ptr, end.offset - begin.offset)
// what is the meaning of this?
// da heck, why did i order of ptr and offset, now I have to think which order is better -_-
//
// ++ increments both ptr and offset
// how to jump to next row? maybe with a reset function? or an advance function?
// even increment not so simple in more dimensions so maye just use a separate step and +=, instead of increment
//
// basic missing concept is a vector with different type of coordinates with reduced set of operations and interoperability with normal numeric vectors
// might be worth it's own template, since the difference with generic vector is vast, though there must be a lowest common denominator
// TODO: implement, generalize to N dimensions
// compare to Boost.GIL which stores a pointer and pitch, various advantages and disatvantages, reimplement all the examples there!!!
// mixing does not make sense, order maybe does, but ptr is always out of order
