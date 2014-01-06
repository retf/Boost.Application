
//               Copyright Ben Robinson 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//#define BOOST_TEST_MAIN defined
#include <boost/test/unit_test.hpp>
#include <boost/ref.hpp>
#include <boost/noncopyable.hpp>
#include <singularity.hpp>

namespace {

using ::boost::singularity;
using ::boost::single_threaded;
using ::boost::multi_threaded;
using ::boost::noncopyable;
using ::boost::cref;

// Some generic, non POD class.
class Event : private noncopyable {
public:
    Event() : mValue(0) {}
    Event(int xValue) : mValue(xValue) {}
    int mValue;
};

// This class demonstrates making itself a Singularity,
// by making its constructors private, and friending
// the Singularity.
class Horizon : private noncopyable {
private:
    Horizon()                     : mEvent(0), mInt(0),    mEventRef(mEvent), mEventPtr(&mEvent), mConstEventRef(mEvent), mConstEventPtr(&mEvent) {}
    Horizon(int xInt)             : mEvent(0), mInt(xInt), mEventRef(mEvent), mEventPtr(&mEvent), mConstEventRef(mEvent), mConstEventPtr(&mEvent) {}
    Horizon(Event       & xEvent) : mEvent(0), mInt(0),    mEventRef(xEvent), mEventPtr(&mEvent), mConstEventRef(mEvent), mConstEventPtr(&mEvent) {}
    Horizon(Event       * xEvent) : mEvent(0), mInt(0),    mEventRef(mEvent), mEventPtr(xEvent),  mConstEventRef(mEvent), mConstEventPtr(&mEvent) {}
    Horizon(Event const & xEvent) : mEvent(0), mInt(0),    mEventRef(mEvent), mEventPtr(&mEvent), mConstEventRef(xEvent), mConstEventPtr(&mEvent) {}
    Horizon(Event const * xEvent) : mEvent(0), mInt(0),    mEventRef(mEvent), mEventPtr(&mEvent), mConstEventRef(mEvent), mConstEventPtr(xEvent)  {}
    Horizon(int xInt, Event & xEventRef, Event * xEventPtr,
        Event const & xConstEventRef, Event const * xConstEventPtr)
        :   mEvent(0), mInt(xInt), mEventRef(xEventRef), mEventPtr(xEventPtr),
            mConstEventRef(xConstEventRef), mConstEventPtr(xConstEventPtr) {}

    Event mEvent; // Used only to initialize the event references when the constructor does not supply one.

public:
	~Horizon() {}
    int           mInt;
    Event &       mEventRef;
    Event *       mEventPtr;
    Event const & mConstEventRef;
    Event const * mConstEventPtr;

private:
    FRIEND_CLASS_SINGULARITY;
};

BOOST_AUTO_TEST_CASE(passOneArgumentByNonConstLValue) {
    int value = 1;
    Horizon & horizon = singularity<Horizon>::create(value);
    BOOST_CHECK_EQUAL(horizon.mInt, 1);
    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(passOneArgumentByConstLValue) {
    int const value = 2;
    Horizon & horizon = singularity<Horizon>::create(value);
    BOOST_CHECK_EQUAL(horizon.mInt, 2);
    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(passOneArgumentByNonConstRValue) {
    Horizon & horizon = singularity<Horizon>::create(3);
    BOOST_CHECK_EQUAL(horizon.mInt, 3);
    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(passOneArgumentByReference) {
    Event event(4);
    Horizon & horizon = singularity<Horizon>::create(event);
    BOOST_CHECK_EQUAL(horizon.mEventRef.mValue, 4);
    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(passOneArgumentByConstReference) {
    Event const event(5);
    Horizon & horizon = singularity<Horizon>::create(event);
    BOOST_CHECK_EQUAL(horizon.mConstEventRef.mValue, 5);
    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(passOneArgumentByAddress) {
    Event event(6);
    Horizon & horizon = singularity<Horizon>::create(&event);
    BOOST_CHECK_EQUAL(horizon.mEventPtr->mValue, 6);
    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(passOneArgumentByConstAddress) {
    Event const event(7);
    Horizon & horizon = singularity<Horizon>::create(&event);
    BOOST_CHECK_EQUAL(horizon.mConstEventPtr->mValue, 7);
    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(passFiveArgumentsAvoidingNonConstRValues) {
    Event event(11);
    Horizon & horizon = singularity<Horizon>::create(cref(10), event, cref(&event), event, cref(&event));
    BOOST_CHECK_EQUAL(horizon.mInt, 10);
    BOOST_CHECK_EQUAL(horizon.mEventRef.mValue, 11);
    BOOST_CHECK_EQUAL(horizon.mEventPtr->mValue, 11);
    BOOST_CHECK_EQUAL(horizon.mConstEventRef.mValue, 11);
    BOOST_CHECK_EQUAL(horizon.mConstEventPtr->mValue, 11);
    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(shouldThrowIfCreateOrDestroyCalledTwice) {
    Horizon & horizon = singularity<Horizon>::create();
    (void)horizon;
    BOOST_CHECK_THROW( // Call create() twice in a row
        Horizon & horizon2 = singularity<Horizon>::create(),
        boost::singularity_already_created
    );

    singularity<Horizon>::destroy();
    BOOST_CHECK_THROW( // Call destroy() twice in a row
        singularity<Horizon>::destroy(),
        boost::singularity_already_destroyed
    );
}

BOOST_AUTO_TEST_CASE(shouldThrowIfAnyCreateCalledTwice) {
    Horizon & horizon = singularity<Horizon>::create();
    (void)horizon;
    BOOST_CHECK_THROW( // Call create() twice in a row
        Horizon & horizon2 = (singularity<Horizon, multi_threaded>::create(2)),
        boost::singularity_already_created
    );

    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(callCreateAgainAfterPreviousIsDestroyed) {
    Horizon & horizon = singularity<Horizon>::create();
    (void)horizon;
    singularity<Horizon>::destroy();
    Horizon & tomorrows_horizon = singularity<Horizon>::create();
    (void)tomorrows_horizon;
    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(demonstrateProperUseOfGet) {

    Horizon & horizon = singularity<Horizon>::create_global();
    (void)horizon;

    Horizon & SameHorizon = singularity<Horizon>::get_global();
    (void)SameHorizon;

    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(shouldThrowOnGetIfNotEnabled) {
    Horizon & horizon = singularity<Horizon>::create();
    (void)horizon;

    BOOST_CHECK_THROW(
        Horizon & inaccessibleHorizon = singularity<Horizon>::get_global(),
        boost::singularity_no_global_access
    );

    singularity<Horizon>::destroy();
}

BOOST_AUTO_TEST_CASE(shouldThrowOnGetBeforeCreate) {
    BOOST_CHECK_THROW(
        Horizon & horizon1 = singularity<Horizon>::get_global(),
        boost::singularity_no_global_access
    );
}

BOOST_AUTO_TEST_CASE(shouldThrowOnGetAfterDestroy) {
    Horizon & horizon = singularity<Horizon>::create_global();
    (void)horizon;

    singularity<Horizon>::destroy();
    BOOST_CHECK_THROW(
        Horizon & noHorizon = singularity<Horizon>::get_global(),
        boost::singularity_not_created
    );
}

BOOST_AUTO_TEST_CASE(demonstrateMultiThreadedUsage) {
    Horizon & horizon = singularity<Horizon, multi_threaded>::create();
    (void)horizon;
    singularity<Horizon, multi_threaded>::destroy();
}

BOOST_AUTO_TEST_CASE(demonstrateGlobalMultiThreadedUsage) {
    typedef singularity<Horizon, multi_threaded> singularityType;

    Horizon & horizon = singularityType::create_global();
    (void)horizon;

    Horizon & SameHorizon = singularityType::get_global();
    (void)SameHorizon;

    singularityType::destroy();
}

} // namespace anonymous
