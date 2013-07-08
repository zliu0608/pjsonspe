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
  