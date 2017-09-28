/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 5 End-User License
   Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.juce.com/juce-5-licence
   Privacy Policy: www.juce.com/juce-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

/**
    Provides in-app purchase functionality.

    Your app should create a single instance of this class, and on iOS it should
    be created as soon as your app starts. This is because on application startup
    any previously pending transactions will be resumed.

    Once an InAppPurchases object is created, call addListener() to attach listeners.
*/
class JUCE_API  InAppPurchases
{
public:
    //==============================================================================
    /** Represents a product available in the store. */
    struct Product
    {
        /** Product ID (also known as SKU) that uniquely identifies a product in the store. */
        String identifier;

        /** Title of the product. */
        String title;

        /** Description of the product. */
        String description;

        /** Price of the product in local currency. */
        String price;

        /** Price locale. */
        String priceLocale;
    };

    //==============================================================================
    /** Represents a purchase of a product in the store. */
    struct Purchase
    {
        /** A unique order identifier for the transaction (generated by the store). */
        String orderId;

        /** A unique identifier of in-app product that was purchased. */
        String productId;

        /** This will be bundle ID on iOS and package name on Android, of the application for which this
            in-app product was purchased. */
        String applicationBundleName;

        /** Date of the purchase (in ISO8601 format). */
        String purchaseTime;

        /** Android only: purchase token that should be used to consume purchase, provided that In-App product
            is consumable. */
        String purchaseToken;
    };

    //==============================================================================
    /** iOS only: represents in-app purchase download. Download will be available only
        for purchases that are hosted on the AppStore. */
    struct Download
    {
        enum class Status
        {
            waiting = 0,    /**< The download is waiting to start. Called at the beginning of a download operation. */
            active,         /**< The download is in progress. */
            paused,         /**< The download was paused and is awaiting resuming or cancelling. */
            finished,       /**< The download was finished successfully. */
            failed,         /**< The download failed (e.g. because of no internet connection). */
            cancelled,      /**< The download was cancelled. */
        };

        virtual ~Download() {}

        /** A unique identifier for the in-app product to be downloaded. */
        virtual String getProductId() const = 0;

        /** Content length in bytes. */
        virtual int64 getContentLength() const = 0;

        /** Content version. */
        virtual String getContentVersion() const = 0;

        /** Returns current status of the download. */
        virtual Status getStatus() const = 0;
    };


    //==============================================================================
    /** Represents an object that gets notified about events such as product info returned or product purchase
        finished. */
    struct Listener
    {
        virtual ~Listener() {}

        /** Called whenever a product info is returned after a call to InAppPurchases::getProductsInformation(). */
        virtual void productsInfoReturned (const Array<Product>& /*products*/) {}

        struct PurchaseInfo
        {
            Purchase purchase;
            Array<Download*> downloads;
        };

        /** Called whenever a purchase is complete, with additional state whether the purchase completed successfully.

            For hosted content (iOS only), the downloads array within PurchaseInfo will contain all download objects corresponding
            with the purchase. For non-hosted content, the downloads array will be empty.

            InAppPurchases class will own downloads and will delete them as soon as they are finished.

            NOTE: it is possible to receive this callback for the same purchase multiple times. If that happens,
            only the newest set of downloads and the newest orderId will be valid, the old ones should be not used anymore!
        */
        virtual void productPurchaseFinished (const PurchaseInfo&, bool /*success*/, const String& /*statusDescription*/) {}

        /** Called when a list of all purchases is restored. This can be used to figure out to
            which products a user is entitled to.

            NOTE: it is possible to receive this callback for the same purchase multiple times. If that happens,
            only the newest set of downloads and the newest orderId will be valid, the old ones should be not used anymore!
        */
        virtual void purchasesListRestored (const Array<PurchaseInfo>&, bool /*success*/, const String& /*statusDescription*/) {}

        /** Called whenever a product consumption finishes. */
        virtual void productConsumed (const String& /*productId*/, bool /*success*/, const String& /*statusDescription*/) {}

        /** iOS only: Called when a product download progress gets updated. If the download was interrupted in the last
            application session, this callback may be called after the application starts.

            If the download was in progress and the application was closed, the download may happily continue in the
            background by OS. If you open the app and the download is still in progress, you will receive this callback.
            If the download finishes in the background before you start the app again, you will receive productDownloadFinished
            callback instead. The download will only stop when it is explicitly cancelled or when it is finished.
        */
        virtual void productDownloadProgressUpdate (Download&, float /*progress*/, RelativeTime /*timeRemaining*/) {}

        /** iOS only: Called when a product download is paused. This may also be called after the application starts, if
            the download was in a paused state and the application was closed before finishing the download.

            Only after the download is finished successfully or cancelled you will stop receiving this callback on startup.
        */
        virtual void productDownloadPaused (Download&) {}

        /** iOS only: Called when a product download finishes (successfully or not). Call Download::getStatus()
            to check if the downloaded finished successfully.

            It is your responsibility to move the download content into your app directory and to clean up
            any files that are no longer needed.

            After the download is finished, the download object is destroyed and should not be accessed anymore.
        */
        virtual void productDownloadFinished (Download&, const URL& /*downloadedContentPath*/) {}
    };

    //==============================================================================
    /** Checks whether in-app purchases is supported on current platform. On iOS this always returns true. */
    bool isInAppPurchasesSupported() const;

    /** Asynchronously requests information for products with given ids. Upon completion, for each enquired product
        there is going to be a corresponding @class Product object.
        If there is no information available for the given product identifier, it will be ignored.
     */
    void getProductsInformation (const StringArray& productIdentifiers);

    /** Asynchronously requests to buy a product with given id.

        @param isSubscription                  (Android only) defines if a product a user wants to buy is a subscription or a one-time purchase.
                                               On iOS, type of the product is derived implicitly.

        @param upgradeOrDowngradeFromSubscriptionsWithProductIdentifiers (Android only) specifies subscriptions that will be replaced by the
                                                                         one being purchased now. Used only when buying a subscription
                                                                         that is an upgrade or downgrade from other ones.

        @param creditForUnusedSubscription     (Android only) controls whether a user should be credited for any unused subscription time on
                                               the products that are being upgraded or downgraded.
    */
    void purchaseProduct (const String& productIdentifier,
                          bool isSubscription,
                          const StringArray& upgradeOrDowngradeFromSubscriptionsWithProductIdentifiers = {},
                          bool creditForUnusedSubscription = true);

    /** Asynchronously asks about a list of products that a user has already bought. Upon completion, Listener::purchasesListReceived()
        callback will be invoked. The user may be prompted to login first.

        @param includeDownloadInfo      (iOS only) if true, then after restoration is successfull, the downloads array passed to
                                        Listener::purchasesListReceived() callback will contain all the download objects corresponding with
                                        the purchase. In the opposite case, the downloads array will be empty.

        @param subscriptionsSharedSecret    (iOS only) required when not including download information and when there are
                                            auto-renewable subscription set up with this app. Refer to In-App-Purchase settings in the store.
    */
    void restoreProductsBoughtList (bool includeDownloadInfo, const juce::String& subscriptionsSharedSecret = {});

    /** Android only: asynchronously sends a request to mark a purchase with given identifier as consumed.
        To consume a product, provide product identifier as well as a purchase token that was generated when
        the product was purchased. The purchase token can also be retrieved by using getProductsInformation().
        In general if it is available on hand, it is better to use it, because otherwise another async
        request will be sent to the store, to first retrieve the token.

        After successful consumption, a product will no longer be returned in getProductsBought() and
        it will be available for purchase.

        On iOS consumption happens automatically. If the product was set as consumable, this function is a no-op.
    */
    void consumePurchase (const String& productIdentifier, const String& purchaseToken = {});

    //==============================================================================
    /** Adds a listener. */
    void addListener (Listener*);

    /** Removes a listener. */
    void removeListener (Listener*);

    //==============================================================================
    /** iOS only: Starts downloads of hosted content from the store. */
    void startDownloads  (const Array<Download*>& downloads);

    /** iOS only: Pauses downloads of hosted content from the store. */
    void pauseDownloads  (const Array<Download*>& downloads);

    /** iOS only: Resumes downloads of hosted content from the store. */
    void resumeDownloads (const Array<Download*>& downloads);

    /** iOS only: Cancels downloads of hosted content from the store. */
    void cancelDownloads (const Array<Download*>& downloads);

    //==============================================================================
   #ifndef DOXYGEN
    InAppPurchases();
    ~InAppPurchases();
   #endif

private:
    //==============================================================================
    ListenerList<Listener> listeners;

   #if JUCE_ANDROID
    friend void juce_inAppPurchaseCompleted (void*);
   #endif

   #if JUCE_ANDROID || JUCE_IOS
    struct Pimpl;
    friend struct Pimpl;

    ScopedPointer<Pimpl> pimpl;
   #endif
};

} // namespace juce
