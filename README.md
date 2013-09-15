Parallel JSON SPE
==============

An auto-Parallelizing JSON streaming process engine

Maintainer
==============
[Zhihui Liu] (https://github.com/zliu0608)

Features
==============

This is a streaming process engine with following aims
- Provide a simple, yet powerful language to manipulate semi-structured data (JSON). 
- Provide an easy of use parallelism for scaling.
- Support both stateless and stateful queries.
- Support multiple type of partition policies.


Changelog
==============

### v0.2  (15-Sep-2013)
- added 3 partition policies on to input stream operator
   - shuffle   roundrobin partition
   - all       no partition, copy all events to all instances
   - expr      partition by a hash value of a expr stringfied result
- added sliding timebased window operator
- added join operator
   - support m-way inner join
   - support support stream outter join with window
   - support hash index, union index, conditional index
- added parallel support more pipeline patterns 
   - join
   - join and then aggregate
- added few more built-in scala functions


### v0.1  (08-July-2013)

- Initial check-in.
- Implement below JSON expressions
   - Literal value expression
   - Arithmetic expression
   - Comparison expression
   - Logic expression
   - Path expression
   - Projection expression
   - Object construct expression
   - Array construct expression
- Implement following core operators
   - filter
   - transform
   - window (sliding count based window only for now)
   - group
   - reduce
   - connect point
  