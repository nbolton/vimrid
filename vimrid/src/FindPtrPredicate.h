/* TODO: Short description of file.
 *
 * TODO: Write detailed information.
 *
 * Created: 05-May-2009
 * Author: Nick Bolton (njb4@aber.ac.uk)
 */

#ifndef FINDPTRPREDICATE_H_
#define FINDPTRPREDICATE_H_

template <typename T>
struct FindPtrPredicate
{
    T mFind;
    bool operator()(T other) const
    {
        return mFind == other;
    }
};

#endif // FINDPTRPREDICATE_H_
